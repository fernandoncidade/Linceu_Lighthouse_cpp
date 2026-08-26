#include "ic_08_Internacionalizador.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QProcess>

#include "ic_01_InterfaceMonitor.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::gui {
namespace {

QDir obter_diretorio_locale() {
    const QString file_dir = QFileInfo(QString::fromUtf8(__FILE__)).absolutePath();
    const QString source_dir = QDir(file_dir).absoluteFilePath("..");
    const QString project_root = QDir(source_dir).absoluteFilePath("..");

    const QString candidate_source = QDir(source_dir).filePath("locale");
    const QString candidate_root = QDir(project_root).filePath("locale");

    if (QDir(candidate_source).exists()) {
        return QDir(candidate_source);
    }
    return QDir(candidate_root);
}

}  // namespace

bool Internacionalizador::inicializar_sistema_traducao(QApplication* app, const QString& idioma_inicial) {
    Q_UNUSED(app)
    try {
        QLocale::setDefault(QLocale(idioma_inicial));

        QDir locale_dir = obter_diretorio_locale();
        if (!locale_dir.exists()) {
            locale_dir.mkpath(QStringLiteral("."));
        }
        return true;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar sistema de tradução", true);
        return false;
    }
}

void Internacionalizador::atualizar_tradutor_qt(InterfaceMonitor* interface_monitor, const QString& idioma) {
    try {
        if (interface_monitor != nullptr && interface_monitor->loc != nullptr) {
            interface_monitor->loc->set_idioma(idioma);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar tradutor Qt", true);
    }
}

bool Internacionalizador::inicializar_tradutor_qt(QApplication* app, const QString& idioma) {
    try {
        return inicializar_sistema_traducao(app, idioma);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar tradutor Qt", true);
        return false;
    }
}

void Internacionalizador::extrair_strings_para_traducao() {
    try {
        const QString file_dir = QFileInfo(QString::fromUtf8(__FILE__)).absolutePath();
        const QString base_dir = QDir(file_dir).absoluteFilePath("../..");
        const QString locale_dir = QDir(base_dir).filePath("locale");
        const QString pro_file = QDir(locale_dir).filePath("linceu.pro");

        if (!QFileInfo::exists(pro_file)) {
            source::utils::LogManager::get_logger().error(
                QString("Arquivo .pro não encontrado: %1").arg(pro_file));
            return;
        }

        QProcess process;
        process.setWorkingDirectory(locale_dir);
        process.start("lupdate", QStringList{pro_file});
        process.waitForFinished(-1);

        if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
            source::utils::LogManager::get_logger().error(
                QString("lupdate retornou código %1: %2")
                    .arg(process.exitCode())
                    .arg(QString::fromUtf8(process.readAllStandardError())),
                true);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao extrair strings", true);
    }
}

void Internacionalizador::compilar_traducoes() {
    try {
        const QString file_dir = QFileInfo(QString::fromUtf8(__FILE__)).absolutePath();
        const QString base_dir = QDir(file_dir).absoluteFilePath("../..");
        const QString locale_dir = QDir(base_dir).filePath("locale");

        QDir dir(locale_dir);
        const QStringList ts_files = dir.entryList(QStringList() << "*.ts", QDir::Files);
        for (const QString& ts_file : ts_files) {
            const QString ts_path = dir.filePath(ts_file);
            const QString qm_path = dir.filePath(ts_file.left(ts_file.size() - 3) + ".qm");

            QProcess process;
            process.start("lrelease", QStringList{ts_path, "-qm", qm_path});
            process.waitForFinished(-1);

            if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
                source::utils::LogManager::get_logger().error(
                    QString("Erro ao compilar %1: %2")
                        .arg(ts_file, QString::fromUtf8(process.readAllStandardError())),
                    true);
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao compilar traduções", true);
    }
}

}  // namespace source::gui
