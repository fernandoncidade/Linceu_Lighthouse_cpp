#include "glqt_03_carregar_tradutor.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QTranslator>

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoLocalizadorQt {

bool carregar_tradutor(source::ui::LocalizadorQt* self, const QString& idioma) {
    try {
        auto* app = QApplication::instance();
        if (app == nullptr) {
            source::utils::LogManager::get_logger().error("QApplication não está disponível");
            return false;
        }

        app->removeTranslator(self->translator_);
        app->removeTranslator(self->qt_translator_);

        const QString translation_file = QDir(self->translations_dir_).filePath(QString("linceu_%1.qm").arg(idioma));
        if (QFileInfo::exists(translation_file)) {
            if (self->translator_->load(translation_file)) {
                app->installTranslator(self->translator_);
            } else {
                source::utils::LogManager::get_logger().warning(
                    QString("Falha ao carregar tradutor da aplicação para %1").arg(idioma));
            }
        } else {
            source::utils::LogManager::get_logger().warning(
                QString("Arquivo de tradução não encontrado: %1").arg(translation_file));
            QDir().mkpath(self->translations_dir_);
        }

        const QLocale qt_locale(idioma);
        QLocale::setDefault(qt_locale);
        const QString qt_translations_path = QDir(self->translations_dir_).filePath("qt");
        if (QDir(qt_translations_path).exists()) {
            if (self->qt_translator_->load(qt_locale, "qtbase", "_", qt_translations_path)) {
                app->installTranslator(self->qt_translator_);
            }
        }
        return true;
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao carregar tradutor para %1").arg(idioma), true);
        return false;
    }
}

}
