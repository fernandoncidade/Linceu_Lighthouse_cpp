#include "gdc_04_abrir_seletor_avancado.h"

#include <memory>

#include <QApplication>
#include <QColorDialog>
#include <QCoreApplication>
#include <QDir>
#include <QFrame>
#include <QHash>
#include <QLocale>
#include <QPointer>
#include <QTimer>
#include <QTranslator>
#include <QVariant>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

#include "gdc_06_traduzir_dialogo_cores.h"
#include "source/ui/ui_11_DialogoCores.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoDialogoCores {

void abrir_seletor_avancado(source::ui::DialogoPaletaCores* self) {
    try {
        if (self == nullptr) {
            return;
        }

        if (self->dialogo_avancado_ != nullptr) {
            if (self->dialogo_avancado_->isVisible()) {
                self->dialogo_avancado_->show();
                self->dialogo_avancado_->raise();
                self->dialogo_avancado_->activateWindow();
                return;
            }

            self->dialogo_avancado_->deleteLater();
            self->dialogo_avancado_ = nullptr;
            self->setProperty("_dialogo_cor_avancado", QVariant::fromValue(static_cast<QObject*>(nullptr)));
        }

        auto* app = QApplication::instance();
        if (app == nullptr) {
            return;
        }

        const QHash<QString, QString> locale_map = {
            {"pt_BR", "pt_BR"}, {"en_US", "en_US"}, {"es_ES", "es_ES"},
            {"fr_FR", "fr_FR"}, {"it_IT", "it_IT"}, {"de_DE", "de_DE"}
        };

        const QString idioma_atual = (self->loc_obj() != nullptr)
                                         ? self->loc_obj()->property("idioma_atual").toString()
                                         : QString("en_US");
        const QString locale_code = locale_map.value(idioma_atual, "en_US");

        QStringList translations_paths;
        if (self->loc_obj() != nullptr) {
            const QString translations_dir = self->loc_obj()->property("translations_dir").toString();
            if (!translations_dir.isEmpty()) {
                translations_paths << QDir(translations_dir).filePath("qt");
            }
        }

        translations_paths << QDir(QCoreApplication::applicationDirPath()).filePath("locale/qt")
                           << QDir(QCoreApplication::applicationDirPath()).filePath("translations/qt")
                           << "/usr/share/qt6/translations"
                           << "C:/Qt/Tools/QtCreator/share/qtcreator/translations";

        auto aplicar_qt_translator = [self, app, translations_paths](const QString& lc) {
            try {
                QLocale::setDefault(QLocale(lc));

                if (self->qt_translator_ != nullptr) {
                    app->removeTranslator(self->qt_translator_);
                    self->qt_translator_->deleteLater();
                    self->qt_translator_ = nullptr;
                }

                auto* novo_trans = new QTranslator(app);
                bool carregado = false;
                for (const auto& path : translations_paths) {
                    if (QDir(path).exists() &&
                        (novo_trans->load(QLocale(lc), "qtbase", "_", path) ||
                         novo_trans->load(QLocale(lc), "qt", "_", path))) {
                        carregado = true;
                        break;
                    }
                }

                if (carregado) {
                    app->installTranslator(novo_trans);
                    self->qt_translator_ = novo_trans;
                } else {
                    delete novo_trans;
                }
            } catch (...) {
                source::utils::LogManager::get_logger().error(
                    QString("Erro ao aplicar tradutor Qt para locale %1").arg(lc), true);
            }
        };

        aplicar_qt_translator(locale_code);

        auto* dialogo = new QColorDialog(self->cor_atual_, nullptr);
        self->dialogo_avancado_ = dialogo;
        dialogo->setAttribute(Qt::WA_DeleteOnClose, true);
        self->setProperty("_dialogo_cor_avancado", QVariant::fromValue(static_cast<QObject*>(dialogo)));
        dialogo->setProperty("interface_principal", self->property("interface_principal"));
        dialogo->installEventFilter(self);
        dialogo->setOption(QColorDialog::DontUseNativeDialog, true);
        dialogo->setOption(QColorDialog::ShowAlphaChannel, false);
        dialogo->setWindowTitle(self->get_text("advanced_color_picker"));
        dialogo->setModal(false);
        dialogo->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

        app->processEvents();
        try {
            traduzir_dialogo_cores(self, dialogo);
        } catch (...) {
        }

        auto traducoes_connection = std::make_shared<QMetaObject::Connection>();
        auto cleanup_executado = std::make_shared<bool>(false);

        QObject* loc_obj = self->loc_obj();
        auto* loc_qt = qobject_cast<source::ui::LocalizadorQt*>(loc_obj);
        if (loc_qt != nullptr) {
            try {
                *traducoes_connection = QObject::connect(
                    loc_qt,
                    &source::ui::LocalizadorQt::traducoes_carregadas,
                    self,
                    [self, dialogo_ptr = QPointer<QColorDialog>(dialogo), locale_map, aplicar_qt_translator](const QString&) {
                        try {
                            QColorDialog* dlg = dialogo_ptr.data();
                            if (dlg == nullptr) {
                                return;
                            }

                            const QString idioma = (self->loc_obj() != nullptr)
                                                       ? self->loc_obj()->property("idioma_atual").toString()
                                                       : QString("en_US");
                            const QString locale_code = locale_map.value(idioma, "en_US");
                            aplicar_qt_translator(locale_code);

                            try {
                                traduzir_dialogo_cores(self, dlg);
                            } catch (...) {
                            }

                            try {
                                dlg->setWindowTitle(self->get_text("advanced_color_picker"));
                            } catch (...) {
                            }
                        } catch (...) {
                            source::utils::LogManager::get_logger().error(
                                "Erro no handler de traduções do seletor avançado", true);
                        }
                    });
            } catch (...) {
                source::utils::LogManager::get_logger().error(
                    "Não foi possível conectar sinal de traduções ao seletor avançado", true);
            }
        }

        auto cleanup_dialog_translator =
            [self, app, traducoes_connection, cleanup_executado]() {
                if (*cleanup_executado) {
                    return;
                }
                *cleanup_executado = true;

                try {
                    if (*traducoes_connection) {
                        QObject::disconnect(*traducoes_connection);
                    }
                } catch (...) {
                }

                try {
                    if (self->qt_translator_ != nullptr) {
                        app->removeTranslator(self->qt_translator_);
                        self->qt_translator_->deleteLater();
                        self->qt_translator_ = nullptr;
                    }
                } catch (...) {
                }
            };

        QObject::connect(dialogo, &QColorDialog::colorSelected, self, [self, dialogo](const QColor& color) {
            if (color.isValid()) {
                if (self->preview_nova_ != nullptr) {
                    self->preview_nova_->setStyleSheet(QString("background-color: %1;").arg(color.name()));
                }
                self->cor_selecionada_ = color;
                try {
                    dialogo->close();
                } catch (...) {
                    source::utils::LogManager::get_logger().error("Erro ao fechar diálogo de cor", true);
                }
            }
        });

        QObject::connect(dialogo, &QColorDialog::finished, self, [cleanup_dialog_translator](int) {
            cleanup_dialog_translator();
        });
        QObject::connect(dialogo, &QObject::destroyed, self, [cleanup_dialog_translator](QObject*) {
            cleanup_dialog_translator();
        });
        QObject::connect(dialogo, &QObject::destroyed, self, [self](QObject*) {
            self->dialogo_avancado_ = nullptr;
            self->setProperty("_dialogo_cor_avancado", QVariant::fromValue(static_cast<QObject*>(nullptr)));
        });

        if (idioma_atual != "en_US") {
            QTimer::singleShot(50, self, [self, dialogo]() {
                try {
                    traduzir_dialogo_cores(self, dialogo);
                } catch (...) {
                }
            });
            try {
                traduzir_dialogo_cores(self, dialogo);
            } catch (...) {
            }
        }

        dialogo->show();
        try {
            dialogo->raise();
            dialogo->activateWindow();
        } catch (...) {
            source::utils::LogManager::get_logger().error("Erro ao ajustar z-order do seletor de cor", true);
        }

        QTimer::singleShot(150, self, [dialogo]() {
            try {
                if (!dialogo->isVisible()) {
                    dialogo->exec();
                }
            } catch (...) {
                source::utils::LogManager::get_logger().error("Erro no fallback de exibição do seletor", true);
            }
        });
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao abrir seletor avançado de cores", true);
    }
}

}  // namespace source::ui::GerenciamentoDialogoCores
