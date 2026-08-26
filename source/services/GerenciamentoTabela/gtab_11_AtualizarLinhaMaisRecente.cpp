#include "gtab_11_AtualizarLinhaMaisRecente.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableView>
#include <QDir>
#include <QSqlError>
#include <QTableWidget>

#include "source/services/GerenciamentoTabela/gtab_47_EventTableModel.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/security/secore_04_traduzir_strings_ui.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

namespace {
constexpr int kRawSecurityValueRole = Qt::UserRole + 100;
constexpr int kRawMetadataValueRole = Qt::UserRole + 101;

QSet<QString> campos_traduziveis() {
    return {
        QStringLiteral("tipo_operacao"), QStringLiteral("nome"), QStringLiteral("dir_anterior"), QStringLiteral("dir_atual"),
        QStringLiteral("data_criacao"), QStringLiteral("data_modificacao"), QStringLiteral("data_acesso"), QStringLiteral("tipo"),
        QStringLiteral("mime"),
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

QVariantMap carregar_evento_mais_recente(const QString& db_path, const QString& conn_name) {
    QVariantMap evento_db;
    if (QSqlDatabase::contains(conn_name)) QSqlDatabase::removeDatabase(conn_name);
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name);
        db.setDatabaseName(db_path);
        if (!db.open()) {
            QSqlDatabase::removeDatabase(conn_name);
            return evento_db;
        }

        {
            QSqlQuery query(db);
            if (query.exec(QStringLiteral("SELECT * FROM monitoramento ORDER BY id DESC LIMIT 1;")) && query.next()) {
                QSqlRecord rec = query.record();
                for (int i = 0; i < rec.count(); ++i) {
                    evento_db.insert(rec.fieldName(i), query.value(i));
                }
            }
        }

        db.close();
    }
    QSqlDatabase::removeDatabase(conn_name);
    return evento_db;
}

}

void atualizar_linha_mais_recente(source::services::GerenciadorTabela* self, QObject* tabela_dados, const QVariantMap& evento) {
    try {
        if (!self || !self->interface || !tabela_dados) return;

        if (auto* tabela_view = qobject_cast<QTableView*>(tabela_dados)) {
            if (auto* model = qobject_cast<source::services::GerenciamentoTabela::EventTableModel*>(tabela_view->model())) {
                if (!evento.isEmpty()) {
                    if (!QMetaObject::invokeMethod(self->interface,
                                                   "adicionar_eventos_tabelas_idioma",
                                                   Qt::DirectConnection,
                                                   Q_ARG(QVariantList, QVariantList{evento}))) {
                        model->prepend_event(evento);
                    }
                    return;
                }
                if (!permitir_carga_db(self)) {
                    return;
                }
                const QString db_path = obter_db_path(self);
                if (!db_path.isEmpty()) {
                    const QVariantMap evento_db = carregar_evento_mais_recente(db_path, QStringLiteral("gtab11_view_%1").arg(reinterpret_cast<quintptr>(self)));
                    if (!evento_db.isEmpty()) {
                        if (!QMetaObject::invokeMethod(self->interface,
                                                       "adicionar_eventos_tabelas_idioma",
                                                       Qt::DirectConnection,
                                                       Q_ARG(QVariantList, QVariantList{evento_db}))) {
                            model->prepend_event(evento_db);
                        }
                    }
                }
                return;
            }
        }

        auto* tabela = qobject_cast<QTableWidget*>(tabela_dados);
        if (!tabela) return;

        auto* gc = qobject_cast<source::services::GerenciadorColunas*>(self->interface->property("gerenciador_colunas").value<QObject*>());
        auto* loc = qobject_cast<source::ui::LocalizadorQt*>(self->loc);
        if (!gc || !loc) return;

        QVariantMap evento_origem = evento;
        if (evento_origem.isEmpty()) {
            if (!permitir_carga_db(self)) return;
            const QString db_path = obter_db_path(self);
            if (db_path.isEmpty()) return;
            evento_origem = carregar_evento_mais_recente(db_path, QStringLiteral("gtab11_tbl_%1").arg(reinterpret_cast<quintptr>(self)));
            if (evento_origem.isEmpty()) return;
        }

        tabela->insertRow(0);

        QList<QPair<QString, QVariantMap>> colunas_disponiveis;
        for (auto it = gc->COLUNAS_DISPONIVEIS.constBegin(); it != gc->COLUNAS_DISPONIVEIS.constEnd(); ++it) {
            colunas_disponiveis.append({it.key(), it.value()});
        }
        std::sort(colunas_disponiveis.begin(), colunas_disponiveis.end(), [](const auto& a, const auto& b) {
            return a.second.value("ordem").toInt() < b.second.value("ordem").toInt();
        });

        const QSet<QString> traduziveis = campos_traduziveis();
        QString tipo_operacao_valor;
        for (int col = 0; col < colunas_disponiveis.size(); ++col) {
            const QString key = colunas_disponiveis.at(col).first;
            QString valor = evento_origem.value(key).toString();
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

            auto* item = new QTableWidgetItem(valor);
            if (eh_campo_seguranca) {
                item->setData(kRawSecurityValueRole, valor_bruto);
            } else if (traduziveis.contains(key)) {
                item->setData(kRawMetadataValueRole, valor_bruto);
            }
            tabela->setItem(0, col, item);
        }

        Q_UNUSED(tipo_operacao_valor);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em atualizar_linha_mais_recente", true);
    }

    try {
        if (auto* tabela = qobject_cast<QTableWidget*>(tabela_dados)) {
            tabela->viewport()->update();
        } else if (auto* view = qobject_cast<QTableView*>(tabela_dados)) {
            if (view->viewport()) view->viewport()->update();
        }
        if (self) self->atualizacao_pendente = true;
    } catch (...) {
    }
}
}  // namespace source::services::GerenciamentoTabela
