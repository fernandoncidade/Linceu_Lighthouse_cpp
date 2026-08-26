#include "gtab_10_AtualizarDadosTabela.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidget>
#include <QDir>
#include <QSqlError>

#include "source/services/ob_02_BaseEvento.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/security/secore_04_traduzir_strings_ui.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

namespace {
constexpr int kRawSecurityValueRole = Qt::UserRole + 100;

QSet<QString> campos_traduziveis() {
    return {
        QStringLiteral("tipo_operacao"), QStringLiteral("nome"), QStringLiteral("dir_anterior"), QStringLiteral("dir_atual"),
        QStringLiteral("data_criacao"), QStringLiteral("data_modificacao"), QStringLiteral("data_acesso"), QStringLiteral("tipo"),
        QStringLiteral("size_b"), QStringLiteral("size_kb"), QStringLiteral("size_mb"), QStringLiteral("size_gb"), QStringLiteral("size_tb"),
        QStringLiteral("atributos"), QStringLiteral("autor"), QStringLiteral("dimensoes"), QStringLiteral("duracao"), QStringLiteral("taxa_bits"),
        QStringLiteral("protegido"), QStringLiteral("paginas"), QStringLiteral("linhas"), QStringLiteral("palavras"),
        QStringLiteral("paginas_estimadas"), QStringLiteral("linhas_codigo"), QStringLiteral("total_linhas"), QStringLiteral("slides_estimados"),
        QStringLiteral("arquivos"), QStringLiteral("unzipped_b"), QStringLiteral("unzipped_kb"), QStringLiteral("unzipped_mb"),
        QStringLiteral("unzipped_gb"), QStringLiteral("unzipped_tb"), QStringLiteral("slides"),
        QStringLiteral("binary_file_b"), QStringLiteral("binary_file_kb"), QStringLiteral("binary_file_mb"), QStringLiteral("binary_file_gb"), QStringLiteral("binary_file_tb"),
        QStringLiteral("planilhas"), QStringLiteral("colunas"), QStringLiteral("registros"), QStringLiteral("tabelas")
    };
}

QString normalizar_caminho(const QString& valor) {
    QString out = QDir::toNativeSeparators(valor);
    if (out == QStringLiteral(".")) out.clear();
    return out;
}

QString obter_db_path(source::services::GerenciadorTabela* self) {
    if (!self || !self->interface) return {};
    if (auto* base = self->interface->property("evento_base").value<source::services::BaseEvento*>()) {
        return base->db_path;
    }
    QObject* base_obj = self->interface->property("evento_base").value<QObject*>();
    if (base_obj) {
        QString db_path;
        if (QMetaObject::invokeMethod(base_obj, "db_path", Q_RETURN_ARG(QString, db_path))) return db_path;
        db_path = base_obj->property("db_path").toString();
        if (!db_path.isEmpty()) return db_path;
    }
    const QString db_fallback = self->interface->property("_monitoramento_db_path").toString();
    if (!db_fallback.isEmpty()) {
        return db_fallback;
    }
    return {};
}

bool permitir_carga_db(source::services::GerenciadorTabela* self) {
    return self && self->interface && self->interface->property("_permitir_carga_db").toBool();
}

}

void atualizar_dados_tabela(source::services::GerenciadorTabela* self, QObject* tabela_dados, int row_especifico) {
    try {
        auto* tabela = qobject_cast<QTableWidget*>(tabela_dados);
        if (!self || !self->interface || !tabela) return;
        if (self->interface->property("_trocando_idioma").toBool() ||
            self->interface->property("_traducao_em_execucao").toBool()) {
            return;
        }

        auto* gc = qobject_cast<source::services::GerenciadorColunas*>(self->interface->property("gerenciador_colunas").value<QObject*>());
        auto* loc = qobject_cast<source::ui::LocalizadorQt*>(self->loc);
        if (!gc || !loc) return;
        if (!permitir_carga_db(self)) return;

        const QString db_path = obter_db_path(self);
        if (db_path.isEmpty()) return;

        const QString conn_name = QStringLiteral("gtab10_%1").arg(reinterpret_cast<quintptr>(self));
        if (QSqlDatabase::contains(conn_name)) {
            QSqlDatabase::removeDatabase(conn_name);
        }

        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name);
        db.setDatabaseName(db_path);
        if (!db.open()) {
            source::utils::LogManager::get_logger().error(QString("Erro ao abrir banco em atualizar_dados_tabela: %1").arg(db.lastError().text()));
            QSqlDatabase::removeDatabase(conn_name);
            return;
        }

        QSqlQuery query(db);
        if (row_especifico >= 0) {
            query.prepare(QStringLiteral("SELECT * FROM monitoramento WHERE id = (SELECT id FROM monitoramento ORDER BY id DESC LIMIT 1 OFFSET ?);"));
            query.addBindValue(row_especifico);
            if (!query.exec()) {
                db.close();
                QSqlDatabase::removeDatabase(conn_name);
                return;
            }
        } else {
            query.exec(QStringLiteral("CREATE INDEX IF NOT EXISTS idx_monitoramento_id ON monitoramento(id);"));
            if (!query.exec(QStringLiteral("SELECT * FROM monitoramento ORDER BY id DESC;"))) {
                db.close();
                QSqlDatabase::removeDatabase(conn_name);
                return;
            }
        }

        QList<QVariantMap> registros;
        const QSqlRecord rec = query.record();
        while (query.next()) {
            QVariantMap evento;
            for (int i = 0; i < rec.count(); ++i) {
                evento.insert(rec.fieldName(i), query.value(i));
            }
            registros.append(evento);
        }

        const int total_registros = registros.size();
        if (row_especifico < 0) {
            tabela->setRowCount(total_registros);
        }

        QList<QPair<QString, QVariantMap>> colunas_disponiveis;
        for (auto it = gc->COLUNAS_DISPONIVEIS.constBegin(); it != gc->COLUNAS_DISPONIVEIS.constEnd(); ++it) {
            colunas_disponiveis.append({it.key(), it.value()});
        }
        std::sort(colunas_disponiveis.begin(), colunas_disponiveis.end(), [](const auto& a, const auto& b) {
            return a.second.value("ordem").toInt() < b.second.value("ordem").toInt();
        });

        const QSet<QString> traduziveis = campos_traduziveis();

        for (int idx = 0; idx < registros.size(); ++idx) {
            const int row = row_especifico >= 0 ? row_especifico : idx;
            const QVariantMap evento = registros.at(idx);
            QString tipo_operacao_valor;

            for (int col = 0; col < colunas_disponiveis.size(); ++col) {
                const QString key = colunas_disponiveis.at(col).first;
                QString valor = evento.value(key).toString();
                const QString valor_bruto = valor;
                const bool eh_campo_seguranca = source::security::is_ui_translatable_field(key);

                if (key == QStringLiteral("tipo_operacao") && !valor.isEmpty()) {
                    valor = source::ui::GerenciamentoLocalizadorQt::traduzir_tipo_operacao(loc, valor);
                    tipo_operacao_valor = valor;
                } else if (eh_campo_seguranca) {
                    valor = source::security::traduzir_valor_ui(loc, key, valor);
                } else if (traduziveis.contains(key)) {
                    valor = source::ui::GerenciamentoLocalizadorQt::traduzir_metadados(loc, valor, key);
                }

                if ((key == QStringLiteral("dir_anterior") || key == QStringLiteral("dir_atual")) && !valor.isEmpty()) {
                    valor = normalizar_caminho(valor);
                }

                auto* item = tabela->item(row, col);
                if (!item) {
                    item = new QTableWidgetItem(valor);
                    tabela->setItem(row, col, item);
                } else if (item->text() != valor) {
                    item->setText(valor);
                }

                if (eh_campo_seguranca) {
                    item->setData(kRawSecurityValueRole, valor_bruto);
                }
            }

            Q_UNUSED(tipo_operacao_valor);
        }

        db.close();
        QSqlDatabase::removeDatabase(conn_name);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em atualizar_dados_tabela", true);
    }

    try {
        if (auto* tabela = qobject_cast<QTableWidget*>(tabela_dados)) {
            tabela->viewport()->update();
        }
        if (self) self->atualizacao_pendente = true;
    } catch (...) {
    }
}
}  // namespace source::services::GerenciamentoTabela
