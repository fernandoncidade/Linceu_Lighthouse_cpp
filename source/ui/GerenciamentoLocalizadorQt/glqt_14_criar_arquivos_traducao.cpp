#include "glqt_14_criar_arquivos_traducao.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoLocalizadorQt {

void criar_arquivos_traducao(source::ui::LocalizadorQt* self) {
    try {
        QDir().mkpath(self->translations_dir_);

        QStringList translationFiles;
        for (auto it = self->idiomas_suportados_.begin(); it != self->idiomas_suportados_.end(); ++it) {
            translationFiles << QString("linceu_%1.ts").arg(it.key());
        }

        const QString pro_content = QString(
            "\nSOURCES += ../InterfaceCore/*.py \\\n        ../GerenciamentoUI/*.py \\\n        ../Filtros/*.py \\\n        ../Observador/*.py\n\n"
            "TRANSLATIONS += %1\n\n"
            "CODECFORTR = UTF-8\n")
            .arg(translationFiles.join(' '));

        const QString pro_file = QDir(self->translations_dir_).filePath("linceu.pro");
        QFile file(pro_file);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream out(&file);
            out << pro_content;
            file.close();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar arquivos de tradução", true);
    }
}

}
