#include "geb_04_exportar_para_sqlite.h"

#include <QHash>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSet>
#include <QTableWidget>
#include <QUuid>

#include <stdexcept>

#include "source/ui/ui_01_GerenciadorBotoes.h"
#include "source/utils/LogManager.h"

namespace {
struct ColunaSqlite {
    QString original;
    QString nome_sql;
};

QString normalizar_coluna(const QString& coluna) {
    QString normalizada = coluna.trimmed().toLower();
    normalizada.replace(QChar(0x00A0), QChar(' '));

    QString segura;
    segura.reserve(normalizada.size());
    bool ultimo_foi_underscore = false;

    for (const QChar ch : normalizada) {
        const bool permitido = ch.isLetterOrNumber() || ch == QChar('_');
        if (permitido) {
            segura.append(ch);
            ultimo_foi_underscore = false;
        } else if (!ultimo_foi_underscore) {
            segura.append(QChar('_'));
            ultimo_foi_underscore = true;
        }
    }

    while (segura.startsWith(QChar('_'))) {
        segura.remove(0, 1);
    }
    while (segura.endsWith(QChar('_'))) {
        segura.chop(1);
    }

    if (segura.isEmpty()) {
        segura = QStringLiteral("coluna");
    }
    if (segura.at(0).isDigit()) {
        segura.prepend(QStringLiteral("col_"));
    }

    return segura;
}

QString escapar_identificador_sqlite(QString identificador) {
    identificador.replace("\"", "\"\"");
    return QStringLiteral("\"%1\"").arg(identificador);
}

QList<ColunaSqlite> construir_colunas_sqlite(const QStringList& colunas) {
    QList<ColunaSqlite> resultado;
    QHash<QString, int> contadores;
    QSet<QString> usados;

    for (const QString& coluna : colunas) {
        const QString base = normalizar_coluna(coluna);
        QString nome_sql = base;
        int contador = contadores.value(base, 1);

        while (usados.contains(nome_sql)) {
            ++contador;
            nome_sql = QStringLiteral("%1_%2").arg(base).arg(contador);
        }

        contadores.insert(base, contador);
        usados.insert(nome_sql);
        resultado.append({coluna, nome_sql});
    }

    return resultado;
}

void throw_sql_error(const QString& contexto, const QSqlError& erro) {
    throw std::runtime_error(QString("%1: %2").arg(contexto, erro.text()).toStdString());
}
}

namespace source::ui::GerenciamentoBotoes {

bool exportar_para_sqlite(source::ui::GerenciadorBotoes* self, void* tabela_dados,
                          const QString& nome_arquivo) {
    Q_UNUSED(self)
    try {
        auto* table = static_cast<QTableWidget*>(tabela_dados);
        if (table == nullptr) {
            return false;
        }

        QList<QMap<QString, QString>> dados;
        for (int row = 0; row < table->rowCount(); ++row) {
            QMap<QString, QString> linha;
            for (int col = 0; col < table->columnCount(); ++col) {
                auto* header_item = table->horizontalHeaderItem(col);
                const QString header = header_item ? header_item->text() : QString("col_%1").arg(col);
                auto* item = table->item(row, col);
                linha.insert(header, item ? item->text() : QString());
            }
            dados.append(linha);
        }

        exportar_para_sqlite(self, dados, nome_arquivo);
        return true;
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao exportar para SQLite: %1").arg(QString::fromUtf8(e.what())), true);
        return false;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao exportar para SQLite", true);
        return false;
    }
}

void exportar_para_sqlite(source::ui::GerenciadorBotoes*, const QList<QMap<QString, QString>>& dados,
                          const QString& nome_arquivo, const QStringList& colunas_primeira_linha) {
    const QString connName = QString("export_sqlite_") + QUuid::createUuid().toString(QUuid::WithoutBraces);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
    db.setDatabaseName(nome_arquivo);

    if (!db.open()) {
        const QSqlError erro = db.lastError();
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connName);
        throw_sql_error("Erro ao abrir SQLite", erro);
    }

    if (dados.isEmpty()) {
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connName);
        return;
    }

    if (!db.transaction()) {
        const QSqlError erro = db.lastError();
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connName);
        throw_sql_error("Erro ao iniciar transação SQLite", erro);
    }

    try {
        {
            QSqlQuery query(db);
            if (!query.exec("DROP TABLE IF EXISTS eventos")) {
                throw_sql_error("Erro ao recriar tabela SQLite", query.lastError());
            }

            QStringList colunas = colunas_primeira_linha;
            if (colunas.isEmpty()) {
                colunas = dados.first().keys();
            }

            const QList<ColunaSqlite> colunas_sqlite = construir_colunas_sqlite(colunas);
            if (colunas_sqlite.isEmpty()) {
                throw std::runtime_error("Nenhuma coluna disponível para exportação SQLite");
            }

            QStringList defs;
            for (const ColunaSqlite& col : colunas_sqlite) {
                defs << (escapar_identificador_sqlite(col.nome_sql) + " TEXT");
            }

            if (!query.exec(QString("CREATE TABLE eventos (%1)").arg(defs.join(", ")))) {
                throw_sql_error("Erro ao criar tabela SQLite", query.lastError());
            }

            for (const auto& evento : dados) {
                QStringList cols;
                QStringList placeholders;
                for (const ColunaSqlite& col : colunas_sqlite) {
                    cols << escapar_identificador_sqlite(col.nome_sql);
                    placeholders << "?";
                }

                QSqlQuery insert(db);
                if (!insert.prepare(QString("INSERT INTO eventos (%1) VALUES (%2)")
                                        .arg(cols.join(","), placeholders.join(",")))) {
                    throw_sql_error("Erro ao preparar INSERT SQLite", insert.lastError());
                }

                for (const ColunaSqlite& col : colunas_sqlite) {
                    insert.addBindValue(evento.value(col.original));
                }

                if (!insert.exec()) {
                    throw_sql_error("Erro ao inserir dados SQLite", insert.lastError());
                }
            }
        }

        if (!db.commit()) {
            throw_sql_error("Erro ao confirmar transação SQLite", db.lastError());
        }
    } catch (...) {
        db.rollback();
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connName);
        throw;
    }

    db.close();
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(connName);
}

}  // namespace source::ui::GerenciamentoBotoes
