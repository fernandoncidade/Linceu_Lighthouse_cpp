#include "geb_01_selecionar_diretorio.h"

#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

#include <QApplication>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QMetaObject>
#include <QPlainTextEdit>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableView>
#include <QTableWidget>
#include <QTextDocument>
#include <QToolButton>
#include <QUuid>
#include <QVariant>
#include <QVariantMap>
#include <QVBoxLayout>
#include <QWidget>

#include "source/services/GerenciamentoTabela/gtab_47_EventTableModel.h"
#include "source/gui/ic_01_InterfaceMonitor.h"
#include "source/security/secore_02_SecurityManager.h"
#include "source/security/secui_00_helpers.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_08_EventoMovido.h"
#include "source/ui/ui_01_GerenciadorBotoes.h"
#include "source/utils/CaminhoPersistenteUtils.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key) {
    QString text;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key));
    }
    return text;
}

QString get_text_or_fallback(QObject* loc, const QString& key, const QString& fallback) {
    const QString value = get_text(loc, key).trimmed();
    if (value.isEmpty() || value == key) {
        return fallback;
    }
    return value;
}

bool has_method(QObject* obj, const char* signature) {
    return obj != nullptr && obj->metaObject() != nullptr && obj->metaObject()->indexOfMethod(signature) >= 0;
}

QString format_python_style(const QString& text_template, const QString& value) {
    QString formatted = text_template;
    const int placeholder_pos = formatted.indexOf(QStringLiteral("{}"));
    if (placeholder_pos >= 0) {
        formatted.replace(placeholder_pos, 2, value);
    }
    return formatted;
}

QLabel* get_label_property(QObject* interface_obj, const char* property_name) {
    if (interface_obj == nullptr) {
        return nullptr;
    }

    const QVariant prop = interface_obj->property(property_name);
    if (!prop.isValid()) {
        return nullptr;
    }

    QLabel* label = prop.value<QLabel*>();
    if (label == nullptr) {
        label = qobject_cast<QLabel*>(prop.value<QObject*>());
    }
    return label;
}

QString obter_db_monitoramento_path() {
    return QDir(source::utils::obter_caminho_persistente())
        .filePath(QStringLiteral("monitoramento.db"));
}

bool monitoramento_tem_dados(const QString& db_path) {
    if (!QFileInfo::exists(db_path)) {
        return false;
    }

    const QString conn_name = QStringLiteral("geb01_db_check_%1")
                                  .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name);
    db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=5000"));
    db.setDatabaseName(db_path);

    bool tem_dados = false;
    if (db.open()) {
        QSqlQuery query(db);
        if (query.exec(QStringLiteral(
                "SELECT 1 FROM sqlite_master WHERE type = 'table' AND name = 'monitoramento' LIMIT 1")) &&
            query.next()) {
            if (query.exec(QStringLiteral("SELECT 1 FROM monitoramento LIMIT 1"))) {
                tem_dados = query.next();
            }
        }
    }

    db.close();
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(conn_name);
    return tem_dados;
}

void atualizar_flag_dados_salvos(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return;
    }

    const QString db_path = obter_db_monitoramento_path();
    interface_obj->setProperty("_monitoramento_db_path", db_path);
    interface_obj->setProperty("_monitoramento_db_tem_dados", monitoramento_tem_dados(db_path));
}

using ImportProgressCallback = std::function<void(int processados, int total, const QVariantMap& evento)>;

QString formatar_linha_evento_importado(const QVariantMap& evento, int processados) {
    const QString id = evento.value(QStringLiteral("id")).toString();
    const QString tipo = evento.value(QStringLiteral("tipo_operacao")).toString();
    const QString nome = evento.value(QStringLiteral("nome")).toString();
    const QString timestamp = evento.value(QStringLiteral("timestamp")).toString();
    return QStringLiteral("[%1] id=%2 | %3 | %4 | %5")
        .arg(processados)
        .arg(id.isEmpty() ? QStringLiteral("-") : id)
        .arg(tipo.isEmpty() ? QStringLiteral("-") : tipo)
        .arg(nome.isEmpty() ? QStringLiteral("-") : nome)
        .arg(timestamp.isEmpty() ? QStringLiteral("-") : timestamp);
}

QVariantList carregar_eventos_salvos(const QString& db_path, const ImportProgressCallback& progresso_cb = {}) {
    QVariantList eventos;
    if (!QFileInfo::exists(db_path)) {
        return eventos;
    }

    const QString conn_name = QStringLiteral("geb01_db_load_%1")
                                  .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name);
    db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=5000"));
    db.setDatabaseName(db_path);

    if (db.open()) {
        QSqlQuery query(db);
        int total = 0;
        if (query.exec(QStringLiteral(
                "SELECT 1 FROM sqlite_master WHERE type = 'table' AND name = 'monitoramento' LIMIT 1")) &&
            query.next()) {
            if (query.exec(QStringLiteral("SELECT COUNT(*) FROM monitoramento")) && query.next()) {
                total = query.value(0).toInt();
            }
            if (progresso_cb) {
                progresso_cb(0, total, QVariantMap());
            }
        }

        if (total > 0 && query.exec(QStringLiteral("SELECT * FROM monitoramento ORDER BY id DESC"))) {
            const QSqlRecord rec = query.record();
            int processados = 0;
            while (query.next()) {
                QVariantMap evento;
                for (int i = 0; i < rec.count(); ++i) {
                    evento.insert(rec.fieldName(i), query.value(i));
                }
                eventos.append(evento);
                ++processados;

                if (progresso_cb) {
                    progresso_cb(processados, total, evento);
                }

                if (processados % 25 == 0) {
                    QApplication::processEvents();
                }
            }
        } else {
            source::utils::LogManager::get_logger().warning(
                QStringLiteral("Falha ao consultar monitoramento.db: %1").arg(query.lastError().text()));
        }
    } else {
        source::utils::LogManager::get_logger().warning(
            QStringLiteral("Falha ao abrir monitoramento.db para importação: %1").arg(db.lastError().text()));
    }

    db.close();
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(conn_name);
    return eventos;
}

bool importar_eventos_na_interface(QObject* interface_obj, const QVariantList& eventos) {
    if (interface_obj == nullptr) {
        return false;
    }

    QObject* tabela_obj = interface_obj->property("tabela_dados").value<QObject*>();
    auto* tabela_widget = qobject_cast<QTableWidget*>(tabela_obj);
    auto* tabela_view = qobject_cast<QTableView*>(tabela_obj);
    auto* interface_monitor = qobject_cast<source::gui::InterfaceMonitor*>(interface_obj);

    if (tabela_widget != nullptr) {
        QObject* gerenciador_tabela = interface_obj->property("gerenciador_tabela").value<QObject*>();
        if (gerenciador_tabela != nullptr) {
            QMetaObject::invokeMethod(gerenciador_tabela, "atualizar_dados_tabela", Qt::DirectConnection,
                                      Q_ARG(QObject*, static_cast<QObject*>(tabela_widget)),
                                      Q_ARG(int, -1));
        }
        if (tabela_widget->viewport() != nullptr) {
            tabela_widget->viewport()->update();
        }
        QMetaObject::invokeMethod(interface_obj, "atualizar_status", Qt::QueuedConnection);
        return tabela_widget->rowCount() > 0 || !eventos.isEmpty();
    }

    source::services::GerenciamentoTabela::EventTableModel* model = nullptr;
    if (tabela_view != nullptr) {
        model = qobject_cast<source::services::GerenciamentoTabela::EventTableModel*>(tabela_view->model());
    }

    if (model == nullptr && interface_monitor != nullptr) {
        model = interface_monitor->event_table_model;
    }

    if (model == nullptr) {
        source::utils::LogManager::get_logger().warning(
            "Importação falhou: EventTableModel indisponível.");
        return false;
    }

    if (!QMetaObject::invokeMethod(interface_obj,
                                   "substituir_eventos_tabelas_idioma",
                                   Qt::DirectConnection,
                                   Q_ARG(QVariantList, eventos))) {
        model->replace_events(eventos);
    }
    if (tabela_view != nullptr && tabela_view->viewport() != nullptr) {
        tabela_view->viewport()->update();
    }

    QMetaObject::invokeMethod(interface_obj, "atualizar_status", Qt::QueuedConnection);
    return model->rowCount() > 0 || !eventos.isEmpty();
}

QString texto_alerta_importacao(QObject* loc) {
    QString texto = get_text(loc, QStringLiteral("import_saved_db_prompt"));
    if (texto == QStringLiteral("import_saved_db_prompt")) {
        texto =
            QStringLiteral("Constam dados salvos no último Banco de Dados, deseja importá-los?");
    }
    return texto;
}

using ObservadorMap = std::unordered_map<QObject*, std::unique_ptr<source::services::Observador>>;
ObservadorMap g_observadores_cpp;
std::mutex g_observadores_cpp_mutex;

source::services::Observador* obter_observador_cpp(QObject* interface_obj) {
    if (interface_obj != nullptr) {
        auto* security_manager = source::security::ui::get_security_manager(interface_obj);
        if (security_manager != nullptr) {
            return security_manager->primary_observer();
        }
    }

    std::lock_guard<std::mutex> lock(g_observadores_cpp_mutex);
    const auto it = g_observadores_cpp.find(interface_obj);
    if (it == g_observadores_cpp.end()) {
        return nullptr;
    }
    return it->second.get();
}

void remover_observador_cpp(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return;
    }

    if (auto* security_manager = source::security::ui::get_security_manager(interface_obj);
        security_manager != nullptr) {
        security_manager->stop_runtime_observers();
        interface_obj->setProperty("_observador_cpp_ptr", QVariant::fromValue<qulonglong>(0));
        return;
    }

    std::unique_ptr<source::services::Observador> removido;
    {
        std::lock_guard<std::mutex> lock(g_observadores_cpp_mutex);
        const auto it = g_observadores_cpp.find(interface_obj);
        if (it == g_observadores_cpp.end()) {
            return;
        }
        removido = std::move(it->second);
        g_observadores_cpp.erase(it);
    }

    interface_obj->setProperty("_observador_cpp_ptr", QVariant::fromValue<qulonglong>(0));

    if (removido != nullptr) {
        try {
            source::services::parar_sistema_evento(interface_obj);
            if (removido->ativo.load()) {
                removido->parar();
            }
        } catch (...) {
        }
    }
}

source::services::Observador* criar_observador_cpp(QObject* interface_obj, const QString& diretorio) {
    if (interface_obj == nullptr) {
        return nullptr;
    }

    if (auto* security_manager = source::security::ui::get_security_manager(interface_obj);
        security_manager != nullptr) {
        security_manager->configure_primary_directory(diretorio);
        return security_manager->configure_runtime_observers();
    }

    auto callback = [interface_guard = QPointer<QObject>(interface_obj)](const QVariantMap& evento) {
        QObject* interface_viva = interface_guard.data();
        if (interface_viva == nullptr) {
            return;
        }

        try {
            const QVariant evento_var(evento);
            source::services::adicionar_evento(interface_viva, evento_var);
        } catch (...) {
            source::utils::LogManager::get_logger().error("Erro ao encaminhar evento para a interface", true);
        }
    };

    auto novo_observador = std::make_unique<source::services::Observador>(diretorio, std::move(callback));
    novo_observador->interface = interface_obj;

    source::services::Observador* observador_raw = novo_observador.get();
    {
        std::lock_guard<std::mutex> lock(g_observadores_cpp_mutex);
        g_observadores_cpp[interface_obj] = std::move(novo_observador);
    }

    interface_obj->setProperty("_observador_cpp_ptr",
                               QVariant::fromValue<qulonglong>(reinterpret_cast<qulonglong>(observador_raw)));

    if (!interface_obj->property("_observador_cpp_cleanup_connected").toBool()) {
        QObject::connect(interface_obj, &QObject::destroyed, [](QObject* obj) { remover_observador_cpp(obj); });
        interface_obj->setProperty("_observador_cpp_cleanup_connected", true);
    }

    return observador_raw;
}

QObject* obter_ou_criar_observador_qt(QObject* interface_obj, const QString& diretorio) {
    if (interface_obj == nullptr) {
        return nullptr;
    }

    QObject* observador = interface_obj->property("observador").value<QObject*>();
    if (observador != nullptr) {
        return observador;
    }

    bool criado = false;

    QObject* novo_objeto = nullptr;
    criado = QMetaObject::invokeMethod(interface_obj, "criar_observador",
                                       Q_RETURN_ARG(QObject*, novo_objeto),
                                       Q_ARG(QString, diretorio));

    if (criado && novo_objeto != nullptr) {
        observador = novo_objeto;
    }

    if (observador != nullptr) {
        observador->setProperty("interface", QVariant::fromValue(interface_obj));
        interface_obj->setProperty("observador", QVariant::fromValue(observador));
    }

    return observador;
}
}

namespace source::ui::GerenciamentoBotoes {

void selecionar_diretorio(source::ui::GerenciadorBotoes* self) {
    try {
        if (self == nullptr) {
            return;
        }

        auto* interface_obj = self->interface_obj();
        if (interface_obj == nullptr) {
            return;
        }

        auto* loc = self->loc_obj();
        auto* parent = qobject_cast<QWidget*>(interface_obj);

        atualizar_flag_dados_salvos(interface_obj);
        const bool db_tem_dados = interface_obj->property("_monitoramento_db_tem_dados").toBool();
        if (db_tem_dados) {
            QMessageBox msg_box(parent);
            msg_box.setWindowTitle(get_text(loc, "warning"));
            msg_box.setText(texto_alerta_importacao(loc));
            msg_box.setIcon(QMessageBox::Question);
            auto* botao_sim = msg_box.addButton(get_text(loc, "yes"), QMessageBox::YesRole);
            auto* botao_nao = msg_box.addButton(get_text(loc, "no"), QMessageBox::NoRole);
            msg_box.setDefaultButton(qobject_cast<QPushButton*>(botao_nao));
            msg_box.exec();

            if (msg_box.clickedButton() == botao_sim) {
                interface_obj->setProperty("_permitir_carga_db", true);
                const QString db_path = interface_obj->property("_monitoramento_db_path").toString();

                QDialog import_dialog(parent);
                import_dialog.setWindowModality(Qt::WindowModal);
                import_dialog.setWindowTitle(
                    get_text_or_fallback(loc, QStringLiteral("import_saved_db_title"), QStringLiteral("Importando dados salvos")));
                import_dialog.setMinimumWidth(500);

                auto* dialog_layout = new QVBoxLayout(&import_dialog);
                dialog_layout->setContentsMargins(12, 12, 12, 12);
                dialog_layout->setSpacing(8);

                auto* status_label = new QLabel(
                    get_text_or_fallback(loc, QStringLiteral("import_saved_db_preparing"), QStringLiteral("Preparando importação...")),
                    &import_dialog);
                auto* barra_import = new QProgressBar(&import_dialog);
                barra_import->setRange(0, 100);
                barra_import->setValue(0);
                barra_import->setFormat(QStringLiteral("%p%"));
                barra_import->setTextVisible(true);

                auto* toggle_detalhes = new QToolButton(&import_dialog);
                const QString texto_mostrar_detalhes = get_text_or_fallback(
                    loc, QStringLiteral("import_saved_db_show_details"), QStringLiteral("Mostrar detalhes"));
                const QString texto_ocultar_detalhes = get_text_or_fallback(
                    loc, QStringLiteral("import_saved_db_hide_details"), QStringLiteral("Ocultar detalhes"));
                toggle_detalhes->setCheckable(true);
                toggle_detalhes->setChecked(false);
                toggle_detalhes->setToolButtonStyle(Qt::ToolButtonTextOnly);
                toggle_detalhes->setText(texto_mostrar_detalhes);

                auto* detalhes_widget = new QWidget(&import_dialog);
                auto* detalhes_layout = new QVBoxLayout(detalhes_widget);
                detalhes_layout->setContentsMargins(0, 0, 0, 0);
                auto* detalhes_view = new QPlainTextEdit(detalhes_widget);
                detalhes_view->setReadOnly(true);
                detalhes_view->document()->setMaximumBlockCount(4000);
                detalhes_view->setPlaceholderText(
                    get_text_or_fallback(loc, QStringLiteral("import_saved_db_details_placeholder"),
                                         QStringLiteral("Os eventos importados serão exibidos aqui em tempo real.")));
                detalhes_layout->addWidget(detalhes_view);
                detalhes_widget->setVisible(false);

                QObject::connect(toggle_detalhes, &QToolButton::toggled, &import_dialog,
                                 [detalhes_widget, toggle_detalhes, texto_mostrar_detalhes,
                                  texto_ocultar_detalhes, &import_dialog](bool checked) {
                                     detalhes_widget->setVisible(checked);
                                     toggle_detalhes->setText(checked ? texto_ocultar_detalhes
                                                                      : texto_mostrar_detalhes);
                                     import_dialog.adjustSize();
                                 });

                dialog_layout->addWidget(status_label);
                dialog_layout->addWidget(barra_import);
                dialog_layout->addWidget(toggle_detalhes);
                dialog_layout->addWidget(detalhes_widget, 1);

                import_dialog.show();
                QApplication::processEvents();

                const QString texto_carregando = get_text_or_fallback(
                    loc, QStringLiteral("import_saved_db_loading"), QStringLiteral("Carregando dados salvos"));
                const QString texto_concluido = get_text_or_fallback(
                    loc, QStringLiteral("import_saved_db_complete"), QStringLiteral("Importação concluída"));
                int ultimo_percentual = -1;

                const QVariantList eventos = carregar_eventos_salvos(
                    db_path, [&](int processados, int total, const QVariantMap& evento) {
                        if (total <= 0) {
                            status_label->setText(get_text_or_fallback(
                                loc, QStringLiteral("import_saved_db_empty"), QStringLiteral("Nenhum dado encontrado para importação.")));
                            barra_import->setValue(0);
                            QApplication::processEvents();
                            return;
                        }

                        const int percentual = qBound(0, (processados * 100) / total, 100);
                        if (percentual != ultimo_percentual) {
                            barra_import->setValue(percentual);
                            ultimo_percentual = percentual;
                        }
                        status_label->setText(
                            QStringLiteral("%1 %2% (%3/%4)")
                                .arg(texto_carregando)
                                .arg(percentual)
                                .arg(processados)
                                .arg(total));

                        if (!evento.isEmpty()) {
                            detalhes_view->appendPlainText(
                                formatar_linha_evento_importado(evento, processados));
                        }

                        if (processados % 25 == 0 || processados == total) {
                            QApplication::processEvents();
                        }
                    });

                if (!eventos.isEmpty()) {
                    status_label->setText(
                        get_text_or_fallback(loc, QStringLiteral("import_saved_db_applying"),
                                             QStringLiteral("Aplicando dados na interface...")));
                    barra_import->setValue(100);
                    QApplication::processEvents();
                }

                if (!eventos.isEmpty() && importar_eventos_na_interface(interface_obj, eventos)) {
                    status_label->setText(
                        QStringLiteral("%1 (%2)")
                            .arg(texto_concluido)
                            .arg(eventos.size()));
                    detalhes_view->appendPlainText(
                        QStringLiteral("---- %1 ----").arg(status_label->text()));
                    QApplication::processEvents();
                    import_dialog.accept();
                    return;
                }

                import_dialog.reject();

                QMessageBox::warning(
                    parent,
                    get_text(loc, "warning"),
                    get_text_or_fallback(
                        loc,
                        QStringLiteral("import_saved_db_error"),
                        QStringLiteral("Falha ao importar os dados salvos para a interface.")));
            }
        }

        const QString diretorio_selecionado =
            QFileDialog::getExistingDirectory(parent, get_text(loc, "select_dir"));
        if (diretorio_selecionado.isEmpty()) {
            return;
        }

        interface_obj->setProperty("_permitir_carga_db", true);
        interface_obj->setProperty("diretorio_atual", diretorio_selecionado);
        if (auto* security_manager = source::security::ui::get_security_manager(interface_obj);
            security_manager != nullptr) {
            security_manager->configure_primary_directory(diretorio_selecionado);
        }

        if (!QMetaObject::invokeMethod(interface_obj, "atualizar_rotulo_diretorios_monitorados")) {
            auto* rotulo = get_label_property(interface_obj, "rotulo_diretorio");
            if (rotulo != nullptr) {
                QString txt = get_text(loc, "dir_selected");
                txt = format_python_style(txt, diretorio_selecionado);
                rotulo->setText(txt);
            }
        }

        if (has_method(interface_obj, "reiniciar_sistema_monitoramento()")) {
            remover_observador_cpp(interface_obj);
            QMetaObject::invokeMethod(interface_obj, "reiniciar_sistema_monitoramento");
        }

        QObject* observador_qt = obter_ou_criar_observador_qt(interface_obj, diretorio_selecionado);
        if (observador_qt != nullptr) {
            observador_qt->setProperty("diretorio", diretorio_selecionado);
            return;
        }

        source::services::Observador* observador_cpp = obter_observador_cpp(interface_obj);
        if (observador_cpp == nullptr) {
            observador_cpp = criar_observador_cpp(interface_obj, diretorio_selecionado);
        }

        if (observador_cpp != nullptr) {
            observador_cpp->diretorio = diretorio_selecionado;
            observador_cpp->interface = interface_obj;
        }
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao selecionar diretório: %1").arg(QString::fromUtf8(e.what())), true);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao selecionar diretório", true);
    }
}

}  // namespace source::ui::GerenciamentoBotoes
