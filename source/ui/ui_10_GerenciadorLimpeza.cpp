#include "ui_10_GerenciadorLimpeza.h"

#include <QAbstractButton>
#include <QApplication>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QMetaObject>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTableView>
#include <QTableWidget>
#include <QVariant>
#include <QVBoxLayout>

#include <exception>
#include <stdexcept>

#include "source/security/secui_01_SecurityUiController.h"
#include "source/services/GerenciamentoBaseEvento/gbank_02_criar_banco_de_dados.h"
#include "source/services/GerenciamentoBaseEvento/gbank_20_db_writer.h"
#include "source/ui/ui_02_GerenciadorBotoesUI.h"
#include "source/services/GerenciamentoTabela/gtab_47_EventTableModel.h"
#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/utils/CaminhoPersistenteUtils.h"
#include "source/utils/LogManager.h"

namespace {
enum class LimpezaAlvo {
    Cancelar,
    TabelaUI,
    BancoDados,
    BancoSeguranca,
    Ambos
};

QString get_text(QObject* loc, const QString& key) {
    QString text;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key));
    }
    return text;
}

QString get_text_or_fallback(QObject* loc, const QString& key, const QString& fallback) {
    const QString text = get_text(loc, key);
    if (text.isEmpty() || text == key) {
        return fallback;
    }
    return text;
}

QString format_with_one_arg(QString pattern, const QString& arg) {
    const int brace_pos = pattern.indexOf(QStringLiteral("{}"));
    if (brace_pos >= 0) {
        pattern.replace(brace_pos, 2, arg);
        return pattern;
    }
    if (pattern.contains(QStringLiteral("%1"))) {
        return pattern.arg(arg);
    }
    if (pattern.isEmpty()) {
        return arg;
    }
    return QStringLiteral("%1: %2").arg(pattern, arg);
}

void set_text(QObject* target, const QString& text) {
    if (target != nullptr) {
        QMetaObject::invokeMethod(target, "setText", Q_ARG(QString, text));
    }
}

void invoke_if_available(QObject* target, const char* method) {
    if (target != nullptr) {
        QMetaObject::invokeMethod(target, method);
    }
}

source::security::ui::SecurityUiController* get_security_ui_controller(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return nullptr;
    }

    return dynamic_cast<source::security::ui::SecurityUiController*>(
        interface_obj->property("security_ui_controller").value<QObject*>());
}

LimpezaAlvo selecionar_alvo_limpeza(QWidget* parent, QObject* loc) {
    QDialog dialog(parent);
    dialog.setWindowTitle(get_text_or_fallback(loc, "confirm", QStringLiteral("Confirmação")));
    dialog.setModal(true);
    dialog.setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(8);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    auto* descricao = new QLabel(
        get_text_or_fallback(loc, "clear_target_prompt", QStringLiteral("Escolha o que deseja limpar:")),
        &dialog);
    descricao->setWordWrap(true);
    layout->addWidget(descricao);

    LimpezaAlvo alvo_selecionado = LimpezaAlvo::Cancelar;
    QList<QPushButton*> botoes_dialogo;
    auto adicionar_botao = [&](const QString& text, LimpezaAlvo alvo, bool padrao = false) {
        auto* botao = new QPushButton(text, &dialog);
        botao->setAutoDefault(true);
        botao->setDefault(padrao);
        layout->addWidget(botao);
        botoes_dialogo.append(botao);
        QObject::connect(botao, &QPushButton::clicked, &dialog, [&dialog, &alvo_selecionado, alvo]() {
            alvo_selecionado = alvo;
            dialog.accept();
        });
        return botao;
    };

    adicionar_botao(get_text_or_fallback(loc, "clear_ui_db_option",
                                         QStringLiteral("Limpar Tabela, Banco de Segurança e de Dados")),
                    LimpezaAlvo::Ambos,
                    true);
    adicionar_botao(get_text_or_fallback(loc, "clear_db_option",
                                         QStringLiteral("Limpar banco de dados")),
                    LimpezaAlvo::BancoDados);
    adicionar_botao(get_text_or_fallback(loc, "clear_ui_table_option",
                                         QStringLiteral("Limpar tabela da UI")),
                    LimpezaAlvo::TabelaUI);
    adicionar_botao(get_text_or_fallback(loc, "security_clear_database",
                                         QStringLiteral("Limpar banco de segurança")),
                    LimpezaAlvo::BancoSeguranca);
    adicionar_botao(get_text_or_fallback(loc, "cancel", QStringLiteral("Cancelar")),
                    LimpezaAlvo::Cancelar);

    source::ui::GerenciadorBotoesUI::ajustar_largura_botoes_texto(botoes_dialogo, 24);
    dialog.adjustSize();
    dialog.setFixedSize(dialog.sizeHint());
    dialog.exec();
    return alvo_selecionado;
}

void fechar_todas_conexoes_db(const QString& db_path) {
    const QStringList names = QSqlDatabase::connectionNames();
    for (const QString& name : names) {
        bool match = false;
        if (name.startsWith(QStringLiteral("dbwriter_")) ||
            name.startsWith(QStringLiteral("baseevento_")) ||
            name.startsWith(QStringLiteral("limpeza_db_")) ||
            name.startsWith(QStringLiteral("stats_db_")) ||
            name.startsWith(QStringLiteral("import_db_")) ||
            name.startsWith(QStringLiteral("geve_")) ||
            name.startsWith(QStringLiteral("security_monitoramento_"))) {
            match = true;
        }
        if (!match && QSqlDatabase::contains(name)) {
            QSqlDatabase db = QSqlDatabase::database(name, false);
            if (db.isValid() && db.databaseName() == db_path) {
                match = true;
            }
        }
        if (match) {
            {
                QSqlDatabase db = QSqlDatabase::database(name, false);
                if (db.isValid() && db.isOpen()) {
                    db.close();
                }
            }
            QSqlDatabase::removeDatabase(name);
        }
    }
}

bool limpar_banco_dados_direto(const QString& db_path) {
    if (db_path.isEmpty()) {
        return false;
    }

    auto writer = source::services::GerenciamentoBaseEvento::DatabaseWriter::get_instance(db_path);
    if (writer) {
        writer->discard_pending();
        writer->suspend_processing(3000);
    }

    fechar_todas_conexoes_db(db_path);

    QFile::remove(db_path + QStringLiteral("-wal"));
    QFile::remove(db_path + QStringLiteral("-shm"));

    const bool file_removed = QFile::remove(db_path);
    if (file_removed) {
        source::services::GerenciamentoBaseEvento::criar_banco_de_dados_path(db_path);
    } else {
        const QString conn_name = QStringLiteral("limpeza_db_%1")
                                      .arg(reinterpret_cast<quintptr>(db_path.constData()));
        if (QSqlDatabase::contains(conn_name)) {
            QSqlDatabase::removeDatabase(conn_name);
        }

        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name);
            db.setDatabaseName(db_path);
            if (db.open()) {
                QSqlQuery pragma(db);
                pragma.exec(QStringLiteral("PRAGMA busy_timeout=5000"));

                if (db.transaction()) {
                    QSqlQuery q(db);
                    const QStringList tabelas = {
                        QStringLiteral("monitoramento"),
                        QStringLiteral("snapshot"),
                        QStringLiteral("adicionado"),
                        QStringLiteral("excluido"),
                        QStringLiteral("modificado"),
                        QStringLiteral("renomeado"),
                        QStringLiteral("movido"),
                        QStringLiteral("escaneado")
                    };
                    for (const QString& t : tabelas) {
                        q.exec(QStringLiteral("DELETE FROM %1").arg(t));
                        q.exec(QStringLiteral("DELETE FROM sqlite_sequence WHERE name='%1'").arg(t));
                    }
                    db.commit();
                }

                QSqlQuery checkpoint(db);
                checkpoint.exec(QStringLiteral("PRAGMA wal_checkpoint(TRUNCATE)"));
                checkpoint.exec(QStringLiteral("PRAGMA optimize"));
                checkpoint.exec(QStringLiteral("VACUUM"));
                checkpoint.exec(QStringLiteral("PRAGMA wal_checkpoint(TRUNCATE)"));

                db.close();
            }
        }
        QSqlDatabase::removeDatabase(conn_name);

        fechar_todas_conexoes_db(db_path);

        QFile::remove(db_path + QStringLiteral("-wal"));
        QFile::remove(db_path + QStringLiteral("-shm"));
    }

    if (writer) {
        writer->resume_processing();
    }

    return true;
}

void limpar_tabela_ui(QObject* interface_obj) {
    QObject* tabela_dados = interface_obj->property("tabela_dados").value<QObject*>();
    if (tabela_dados != nullptr) {
        if (auto* tabela = qobject_cast<QTableWidget*>(tabela_dados)) {
            tabela->clearContents();
            tabela->setRowCount(0);
            tabela->viewport()->update();
        } else if (auto* view = qobject_cast<QTableView*>(tabela_dados)) {
            if (!QMetaObject::invokeMethod(interface_obj, "limpar_tabelas_idioma")) {
                if (auto* model = qobject_cast<source::services::GerenciamentoTabela::EventTableModel*>(view->model())) {
                    model->replace_events(QVariantList());
                }
            }
            if (view->viewport() != nullptr) {
                view->viewport()->update();
            }
        } else {
            QMetaObject::invokeMethod(tabela_dados, "clearContents");
            QMetaObject::invokeMethod(tabela_dados, "setRowCount", Q_ARG(int, 0));
        }
    }

    interface_obj->setProperty("ultimo_salvamento", QVariant());

    QObject* painel_filtros = interface_obj->property("painel_filtros").value<QObject*>();
    if (auto* filtros = qobject_cast<source::tools::Filtros*>(painel_filtros)) {
        if (auto* administrador = filtros->administrador_filtros_obj()) {
            administrador->limpar_filtros();
        }
    } else {
        QObject* administrador_filtros = (painel_filtros != nullptr)
                                             ? painel_filtros->property("administrador_filtros").value<QObject*>()
                                             : nullptr;
        if (administrador_filtros != nullptr) {
            invoke_if_available(administrador_filtros, "limpar_filtros");
        }
    }

    QObject* barra_progresso = interface_obj->property("barra_progresso").value<QObject*>();
    if (barra_progresso != nullptr) {
        invoke_if_available(barra_progresso, "hide");
        QMetaObject::invokeMethod(barra_progresso, "setValue", Q_ARG(int, 0));
    }
}
}

namespace source::ui {

GerenciadorLimpeza::GerenciadorLimpeza(QObject* interface_principal) : interface_(interface_principal) {}

void GerenciadorLimpeza::limpar_dados() {
    auto* parent = qobject_cast<QWidget*>(interface_);
    auto* loc = interface_->property("loc").value<QObject*>();

    try {
        const LimpezaAlvo alvo = selecionar_alvo_limpeza(parent, loc);
        if (alvo == LimpezaAlvo::Cancelar) {
            return;
        }

        if (alvo == LimpezaAlvo::BancoSeguranca) {
            if (auto* security_ui_controller = get_security_ui_controller(interface_)) {
                security_ui_controller->clear_security_database();
            } else {
                source::utils::LogManager::get_logger().warning(
                    QStringLiteral("SecurityUiController indisponível ao solicitar limpeza do banco de segurança."));
            }
            return;
        }

        QApplication::setOverrideCursor(Qt::WaitCursor);
        try {
            bool observador_ativo = false;
            if (interface_->property("_observador_cpp_ptr").toULongLong() != 0) {
                observador_ativo = true;
            }
            auto* observador = interface_->property("observador").value<QObject*>();
            if (observador != nullptr && observador->property("ativo").isValid()) {
                observador_ativo = observador->property("ativo").toBool();
            }

            const bool limpar_banco = (alvo == LimpezaAlvo::BancoDados || alvo == LimpezaAlvo::Ambos);
            const bool limpar_ui = (alvo == LimpezaAlvo::TabelaUI || alvo == LimpezaAlvo::Ambos);
            const bool limpar_banco_seguranca = (alvo == LimpezaAlvo::Ambos);

            if (limpar_banco && observador_ativo) {
                invoke_if_available(interface_->property("gerenciador_botoes").value<QObject*>(), "alternar_analise_diretorio");
            }

            if (limpar_banco_seguranca) {
                auto* security_ui_controller = get_security_ui_controller(interface_);
                if (security_ui_controller == nullptr) {
                    throw std::runtime_error("SecurityUiController indisponível para limpeza combinada.");
                }

                QString security_error;
                if (!security_ui_controller->clear_security_database(false, false, &security_error)) {
                    throw std::runtime_error(
                        security_error.isEmpty()
                            ? "Falha ao limpar o banco de segurança."
                            : security_error.toUtf8().constData());
                }
            }

            if (limpar_banco) {
                QString db_path = interface_->property("_monitoramento_db_path").toString();
                if (db_path.isEmpty()) {
                    db_path = QDir(source::utils::obter_caminho_persistente())
                                  .filePath(QStringLiteral("monitoramento.db"));
                }

                auto* evento_base = interface_->property("evento_base").value<QObject*>();
                bool limpou_via_evento_base = false;
                if (evento_base != nullptr) {
                    limpou_via_evento_base = QMetaObject::invokeMethod(evento_base, "limpar_registros");
                }

                if (!limpou_via_evento_base) {
                    if (!limpar_banco_dados_direto(db_path)) {
                        source::utils::LogManager::get_logger().warning(
                            QStringLiteral("Falha ao limpar banco de dados diretamente: %1").arg(db_path));
                    }
                }

                interface_->setProperty("_monitoramento_db_tem_dados", false);
                interface_->setProperty("contador_eventos", 0);
                interface_->setProperty("excluidos_recentemente", QVariantMap());
                interface_->setProperty("_scan_contador", 0);
                interface_->setProperty("_scan_total", 0);
                interface_->setProperty("_scan_em_execucao", false);
            }

            if (limpar_ui) {
                limpar_tabela_ui(interface_);
            }

            invoke_if_available(interface_, "atualizar_status");

            QObject* gerenciador_colunas = interface_->property("gerenciador_colunas").value<QObject*>();
            if (gerenciador_colunas != nullptr) {
                invoke_if_available(gerenciador_colunas, "limpar_cache_metadados");
            }

            if (limpar_ui) {
                const QString texto_data_cleared = get_text(loc, "data_cleared");
                set_text(interface_->property("rotulo_resultado").value<QObject*>(), texto_data_cleared);
                set_text(interface_->property("rotulo_contador_eventos").value<QObject*>(),
                         QString("%1: 0").arg(get_text_or_fallback(loc, "events_monitored",
                                                                   QStringLiteral("Eventos Monitorados"))));
            }

            if (QApplication::overrideCursor() != nullptr) {
                QApplication::restoreOverrideCursor();
            }

            QString mensagem_sucesso;
            if (alvo == LimpezaAlvo::TabelaUI) {
                mensagem_sucesso = get_text_or_fallback(
                    loc, "clear_ui_success",
                    QStringLiteral("Tabela da interface limpa com sucesso."));
            } else if (alvo == LimpezaAlvo::BancoDados) {
                mensagem_sucesso = get_text_or_fallback(
                    loc, "clear_db_success",
                    QStringLiteral("Banco de dados limpo com sucesso."));
            } else {
                mensagem_sucesso = get_text_or_fallback(loc, "clear_success",
                                                        QStringLiteral("Dados limpos com sucesso."));
            }
            QMessageBox::information(parent, get_text_or_fallback(loc, "success", QStringLiteral("Sucesso")),
                                     mensagem_sucesso);
        } catch (const std::exception& e) {
            if (QApplication::overrideCursor() != nullptr) {
                QApplication::restoreOverrideCursor();
            }
            source::utils::LogManager::get_logger().error(
                QStringLiteral("Erro ao limpar dados: %1").arg(QString::fromUtf8(e.what())),
                true);
            QMessageBox::critical(parent,
                                  get_text_or_fallback(loc, "error", QStringLiteral("Erro")),
                                  format_with_one_arg(
                                      get_text_or_fallback(loc, "clear_error",
                                                           QStringLiteral("Erro ao limpar dados: %1")),
                                      QString::fromUtf8(e.what())));
        } catch (...) {
            if (QApplication::overrideCursor() != nullptr) {
                QApplication::restoreOverrideCursor();
            }
            source::utils::LogManager::get_logger().error("Erro ao limpar dados", true);
            QMessageBox::critical(parent,
                                  get_text_or_fallback(loc, "error", QStringLiteral("Erro")),
                                  format_with_one_arg(get_text_or_fallback(loc, "clear_error",
                                                                           QStringLiteral("Erro ao limpar dados: %1")),
                                                      QStringLiteral("erro desconhecido")));
        }
    } catch (...) {
        if (QApplication::overrideCursor() != nullptr) {
            QApplication::restoreOverrideCursor();
        }
        source::utils::LogManager::get_logger().error("Erro ao limpar dados", true);
        QMessageBox::critical(parent,
                              get_text_or_fallback(loc, "error", QStringLiteral("Erro")),
                              format_with_one_arg(get_text_or_fallback(loc, "clear_error",
                                                                       QStringLiteral("Erro ao limpar dados: %1")),
                                                  QStringLiteral("erro desconhecido")));
    }

    QApplication::processEvents();
}

}  // namespace source::ui
