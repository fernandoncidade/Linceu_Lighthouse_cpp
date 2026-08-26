#include "src_01_InicializadorMain.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QMetaObject>
#include <QString>
#include <QSurfaceFormat>

#include "src_02_SplashAppStarter.h"
#include "source/data/Charts/ch_04_QmlRegistration.h"
#include "source/gui/ic_01_InterfaceMonitor.h"
#include "source/security/secore_01_postgres_utils.h"
#include "source/gui/ic_08_Internacionalizador.h"
#include "source/services/GerenciamentoBaseEvento/gbank_20_db_writer.h"
#include "source/utils/IconUtils.h"
#include "source/utils/LogManager.h"

#ifdef _WIN32
#include <Windows.h>
#include <shobjidl.h>
#include <shellapi.h>
#endif

namespace {

auto logger = source::utils::LogManager::get_logger();

void ajustar_cwd() {
    try {
        if (QCoreApplication::instance() != nullptr) {
            const QString appDir = QCoreApplication::applicationDirPath();
            if (!appDir.isEmpty()) {
                QDir::setCurrent(appDir);
                return;
            }
        }

        const QFileInfo currentFileInfo(QString::fromUtf8(__FILE__));
        const QString baseDir = currentFileInfo.absoluteDir().absolutePath() + "/..";
        QDir::setCurrent(QDir(baseDir).absolutePath());
    } catch (const std::exception& e) {
        logger.warning(QString("Falha ao ajustar diretório de trabalho: %1").arg(e.what()));
    } catch (...) {
        logger.warning("Falha ao ajustar diretório de trabalho: erro desconhecido");
    }
}

void definir_appusermodelid() {
#ifdef _WIN32
    try {
        ::SetCurrentProcessExplicitAppUserModelID(L"LinceuLighthouse.App");
    } catch (...) {
        logger.warning("Falha ao definir AppUserModelID");
    }
#endif
}

QString normalizar_modo_opengl(const QByteArray& value) {
    const QString raw = QString::fromLatin1(value).trimmed().toLower();
    if (raw.isEmpty()) {
        return QStringLiteral("auto");
    }
    if (raw == QStringLiteral("software") || raw == QStringLiteral("desktop") || raw == QStringLiteral("angle") ||
        raw == QStringLiteral("auto")) {
        return raw;
    }
    return QStringLiteral("auto");
}

QSurfaceFormat criar_formato_opengl_padrao() {
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setSamples(qMax(8, format.samples()));
    return format;
}

void configurar_backend_grafico() {
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
    QSurfaceFormat::setDefaultFormat(criar_formato_opengl_padrao());

#ifdef _WIN32
    const QString modo = normalizar_modo_opengl(qgetenv("LINCEU_OPENGL_MODE"));
    if (modo == QStringLiteral("software")) {
        qputenv("QT_OPENGL", QByteArrayLiteral("software"));
        QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL, true);
        logger.warning("Renderizacao OpenGL em modo software habilitada (LINCEU_OPENGL_MODE=software)");
    } else if (modo == QStringLiteral("desktop")) {
        qputenv("QT_OPENGL", QByteArrayLiteral("desktop"));
        logger.info("Renderizacao OpenGL em modo desktop habilitada (LINCEU_OPENGL_MODE=desktop)");
    } else if (modo == QStringLiteral("angle")) {
        qputenv("QT_OPENGL", QByteArrayLiteral("angle"));
        logger.info("Renderizacao OpenGL em modo ANGLE habilitada (LINCEU_OPENGL_MODE=angle)");
    } else {
        logger.info("Renderizacao OpenGL em modo automatico habilitada (LINCEU_OPENGL_MODE=auto/default)");
    }
#endif
}

}  // namespace

namespace source {

int iniciar_aplicacao(int argc, char* argv[]) {
    try {
        ajustar_cwd();

        try {
            SplashAppStarter starter;
            return starter.start(argc, argv);
        } catch (const std::exception& e) {
            logger.debug(QString("SplashAppStarter não utilizado: %1").arg(e.what()));
        } catch (...) {
            logger.debug("SplashAppStarter não utilizado: erro desconhecido");
        }

        configurar_backend_grafico();
        QApplication app(argc, argv);
        source::charts_lgpl::register_qml_types();
        source::security::pg::prepare_runtime();
        definir_appusermodelid();

        QString appIconPath;
        try {
            appIconPath = source::utils::get_icon_path("linceu_sem_fundo.ico");
            if (!appIconPath.isEmpty() && QFileInfo::exists(appIconPath)) {
                app.setWindowIcon(QIcon(appIconPath));
            } else {
                logger.warning(QString("Ícone do app não encontrado: %1").arg(appIconPath));
            }
        } catch (...) {
            logger.warning("Falha ao definir ícone do aplicativo");
        }

        source::gui::Internacionalizador::inicializar_sistema_traducao(&app);
        auto* window = new source::gui::InterfaceMonitor();

        try {
            if (window->evento_base != nullptr) {
                QString dbPath;
                QMetaObject::invokeMethod(window->evento_base, "db_path", Q_RETURN_ARG(QString, dbPath));
                if (!dbPath.isEmpty()) {
                    source::services::GerenciamentoBaseEvento::DatabaseWriter::get_instance(dbPath);
                }
            }
        } catch (...) {
            logger.warning("Falha ao iniciar DatabaseWriter de forma eager");
        }

        try {
            if (!appIconPath.isEmpty() && QFileInfo::exists(appIconPath)) {
                window->setWindowIcon(QIcon(appIconPath));
            }
        } catch (...) {
        }

        window->show();
        const int exitCode = app.exec();
        logger.debug(QString("Aplicação encerrada com código de saída: %1").arg(exitCode));
        return exitCode;
    } catch (...) {
        logger.critical("Erro fatal ao iniciar aplicação");
        return 1;
    }
}

}  // namespace source
