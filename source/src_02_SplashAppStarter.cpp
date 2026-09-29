#include "src_02_SplashAppStarter.h"

#include <QApplication>
#include <QCoreApplication>
#include <QFileInfo>
#include <QIcon>
#include <QLabel>
#include <QMetaObject>
#include <QPalette>
#include <QPixmap>
#include <QProgressBar>
#include <QSurfaceFormat>
#include <QTextCursor>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "source/data/Charts/ch_04_QmlRegistration.h"
#include "source/gui/ic_01_InterfaceMonitor.h"
#include "source/gui/ic_08_Internacionalizador.h"
#include "source/security/secore_01_postgres_utils.h"
#include "source/services/GerenciamentoBaseEvento/gbank_20_db_writer.h"
#include "source/utils/IconUtils.h"
#include "source/utils/LogManager.h"

#ifdef _WIN32
#include <Windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#endif

namespace source {
namespace {

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
        source::utils::LogManager::get_logger().warning(
            "Renderizacao OpenGL em modo software habilitada (LINCEU_OPENGL_MODE=software)");
    } else if (modo == QStringLiteral("desktop")) {
        qputenv("QT_OPENGL", QByteArrayLiteral("desktop"));
        source::utils::LogManager::get_logger().info(
            "Renderizacao OpenGL em modo desktop habilitada (LINCEU_OPENGL_MODE=desktop)");
    } else if (modo == QStringLiteral("angle")) {
        qputenv("QT_OPENGL", QByteArrayLiteral("angle"));
        source::utils::LogManager::get_logger().info(
            "Renderizacao OpenGL em modo ANGLE habilitada (LINCEU_OPENGL_MODE=angle)");
    } else {
        source::utils::LogManager::get_logger().info(
            "Renderizacao OpenGL em modo automatico habilitada (LINCEU_OPENGL_MODE=auto/default)");
    }
#endif
}

void definir_appusermodelid() {
#ifdef _WIN32
    try {
        ::SetCurrentProcessExplicitAppUserModelID(L"LinceuLighthouse.App");
    } catch (...) {
        source::utils::LogManager::get_logger().warning("Falha ao definir AppUserModelID");
    }
#endif
}

}  // namespace

SplashAppStarter::SplashAppStarter()
    : app_(nullptr),
      splash_(nullptr),
      progress_(nullptr),
      log_widget_(nullptr),
      window_(nullptr) {}

std::optional<QIcon> SplashAppStarter::load_qicon(const QString& path) const {
    try {
        if (path.isEmpty()) {
            return std::nullopt;
        }

        const QString absPath = QFileInfo(path).absoluteFilePath();
        if (!QFileInfo::exists(absPath)) {
            source::utils::LogManager::get_logger().debug(
                QString("Ícone não encontrado no caminho: %1").arg(absPath));
            return std::nullopt;
        }

        QIcon icon(absPath);
        if (!icon.isNull()) {
            return icon;
        }

        QPixmap pix(absPath);
        if (!pix.isNull()) {
            return QIcon(pix);
        }

        source::utils::LogManager::get_logger().debug(
            QString("Falha ao carregar ícone como QIcon/QPixmap: %1").arg(absPath));
        return std::nullopt;
    } catch (...) {
        source::utils::LogManager::get_logger().debug(
            QString("Erro ao carregar ícone '%1'").arg(path));
        return std::nullopt;
    }
}

void SplashAppStarter::create_app(int argc, char* argv[]) {
    configurar_backend_grafico();
    app_ = new QApplication(argc, argv);
    app_->setQuitOnLastWindowClosed(false);
    source::utils::LogManager::get_logger().debug("SplashAppStarter: QApplication criada");

    source::charts_lgpl::register_qml_types();
    source::security::pg::prepare_runtime();
    definir_appusermodelid();

    QObject::connect(app_, &QCoreApplication::aboutToQuit, []() {
        source::utils::LogManager::get_logger().warning("SplashAppStarter: aboutToQuit emitido");
    });

    try {
        const QString appIconPath = source::utils::get_icon_path("linceu_sem_fundo_300_300.png");
        source::utils::LogManager::get_logger().debug(
            QString("Tentando carregar o ícone do aplicativo em: %1").arg(appIconPath));

        const auto icon = load_qicon(appIconPath);
        if (icon.has_value()) {
            app_->setWindowIcon(icon.value());
            source::utils::LogManager::get_logger().info(
                QString("Ícone do aplicativo configurado com sucesso: %1").arg(appIconPath));
        } else {
            source::utils::LogManager::get_logger().warning(
                QString("Ícone do aplicativo não encontrado ou inválido: %1").arg(appIconPath));
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao configurar o ícone do aplicativo");
    }

    try {
        source::gui::Internacionalizador::inicializar_sistema_traducao(app_);
        source::utils::LogManager::get_logger().debug(
            "Sistema de tradução inicializado via Internacionalizador");
    } catch (...) {
        source::utils::LogManager::get_logger().warning("Falha ao inicializar traduções");
    }
}

QWidget* SplashAppStarter::create_splash() {
    QPixmap pix;
    try {
        const QString iconPath = source::utils::get_icon_path("linceu_sem_fundo_300_300.png");
        if (!iconPath.isEmpty()) {
            pix = QPixmap(iconPath);
        }
    } catch (...) {
    }

    if (pix.isNull()) {
        pix = QPixmap(600, 300);
        pix.fill(Qt::white);
    }

    const int origPw = pix.width() > 0 ? pix.width() : 600;
    const int origPh = pix.height() > 0 ? pix.height() : 300;
    const int targetW = qMax(500, origPw * 2);
    const int targetH = qMax(300, origPh * 2);

    const QPixmap scaledPix = pix.scaled(targetW, targetH - 160, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    splash_ = new QWidget();
    splash_->setWindowFlags(Qt::SplashScreen | Qt::FramelessWindowHint);
    splash_->setAttribute(Qt::WA_TranslucentBackground, true);
    splash_->resize(scaledPix.width() + 40, scaledPix.height() + 160);

    auto* layout = new QVBoxLayout(splash_);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(8);

    auto* imgLabel = new QLabel(splash_);
    imgLabel->setPixmap(scaledPix);
    imgLabel->setAlignment(Qt::AlignCenter);
    imgLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    layout->addWidget(imgLabel);

    progress_ = new QProgressBar(splash_);
    progress_->setRange(0, 100);
    progress_->setValue(0);
    progress_->setTextVisible(true);
    progress_->setFixedHeight(18);
    progress_->setMinimumWidth(qMax(200, splash_->width() - 40));
    progress_->setAttribute(Qt::WA_TranslucentBackground, true);
    progress_->setAutoFillBackground(false);
    {
        QPalette pal = progress_->palette();
        pal.setColor(QPalette::Base, Qt::transparent);
        pal.setColor(QPalette::Window, Qt::transparent);
        progress_->setPalette(pal);
    }
    layout->addWidget(progress_);

    log_widget_ = new QTextEdit(splash_);
    log_widget_->setReadOnly(true);
    log_widget_->setFixedHeight(80);
    log_widget_->setAttribute(Qt::WA_TranslucentBackground, true);
    log_widget_->setAutoFillBackground(false);
    {
        QPalette pal2 = log_widget_->palette();
        pal2.setColor(QPalette::Base, Qt::transparent);
        pal2.setColor(QPalette::Window, Qt::transparent);
        log_widget_->setPalette(pal2);
    }
    layout->addWidget(log_widget_);

    if (app_ != nullptr) {
        app_->processEvents();
    }

    source::utils::LogManager::get_logger().debug("SplashAppStarter: splash criado e exibível");

    return splash_;
}

void SplashAppStarter::report_progress(int percent, const QString& message) {
    try {
        if (progress_ != nullptr) {
            progress_->setValue(percent);
        }

        if (!message.isEmpty() && log_widget_ != nullptr) {
            log_widget_->append(message);
            QTextCursor cursor = log_widget_->textCursor();
            cursor.movePosition(QTextCursor::End);
            log_widget_->setTextCursor(cursor);
        }

        if (app_ != nullptr) {
            app_->processEvents();
        }
    } catch (...) {
    }
}

void SplashAppStarter::loader() {
    try {
        source::utils::LogManager::get_logger().debug("SplashAppStarter: loader iniciado");
        report_progress(5, QCoreApplication::translate("App", "Iniciando carregamento..."));

#ifdef _WIN32
        report_progress(40, QCoreApplication::translate("App", "Ambiente Windows detectado"));
#endif

        report_progress(55, QCoreApplication::translate("App", "Importado:") + QString(" source.gui.ic_01_InterfaceMonitor"));
        report_progress(60, QCoreApplication::translate("App", "Importado:") + QString(" source.utils.IconUtils"));
        report_progress(65, QCoreApplication::translate("App", "Importado:") + QString(" source.gui.ic_08_Internacionalizador"));
        report_progress(70, QCoreApplication::translate("App", "Importado:") + QString(" source.services.GerenciamentoBaseEvento.gbank_20_db_writer"));

        try {
            source::gui::Internacionalizador::inicializar_sistema_traducao(app_);
        } catch (...) {
        }

        source::utils::LogManager::get_logger().debug("SplashAppStarter: criando InterfaceMonitor");
        auto* mainWindow = new source::gui::InterfaceMonitor();
        window_ = mainWindow;
        source::utils::LogManager::get_logger().debug("SplashAppStarter: InterfaceMonitor criada");

        try {
            mainWindow->show();
            source::utils::LogManager::get_logger().debug("SplashAppStarter: InterfaceMonitor exibida");
        } catch (...) {
            source::utils::LogManager::get_logger().error("SplashAppStarter: falha ao exibir InterfaceMonitor", true);
        }

        try {
            if (mainWindow->evento_base != nullptr) {
                QString db_path;
                QMetaObject::invokeMethod(mainWindow->evento_base, "db_path", Q_RETURN_ARG(QString, db_path));
                if (!db_path.isEmpty()) {
                    source::services::GerenciamentoBaseEvento::DatabaseWriter::get_instance(db_path);
                }
            }
        } catch (...) {
        }

        try {
            const QString iconPath = source::utils::get_icon_path("linceu_sem_fundo_300_300.png");
            const auto icon = load_qicon(iconPath);
            if (icon.has_value()) {
                mainWindow->setWindowIcon(icon.value());
            }
        } catch (...) {
        }

        if (app_ != nullptr) {
            app_->setQuitOnLastWindowClosed(true);
        }

        report_progress(95, QCoreApplication::translate("App", "Finalizando inicialização..."));

        if (splash_ != nullptr) {
            splash_->close();
        }

        try {
            if (mainWindow != nullptr) {
                mainWindow->raise();
                mainWindow->activateWindow();
            }
        } catch (...) {
        }
        source::utils::LogManager::get_logger().debug("SplashAppStarter: loader finalizado");
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().critical(
            QString("Erro ao carregar aplicação (std::exception): %1").arg(e.what()));
        if (splash_ != nullptr) {
            splash_->close();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().critical("Erro ao carregar aplicação");
        if (splash_ != nullptr) {
            splash_->close();
        }
    }
}

int SplashAppStarter::start(int argc, char* argv[]) {
    source::utils::LogManager::get_logger().debug("SplashAppStarter: start() iniciado");
    create_app(argc, argv);
    create_splash();

    if (splash_ != nullptr) {
        splash_->show();
        source::utils::LogManager::get_logger().debug("SplashAppStarter: splash exibido");
    }

    if (app_ != nullptr) {
        QTimer::singleShot(0, [this]() { loader(); });
        const int exit_code = app_->exec();
        source::utils::LogManager::get_logger().warning(
            QString("SplashAppStarter: app.exec finalizado com código %1").arg(exit_code));
        return exit_code;
    }

    source::utils::LogManager::get_logger().critical("SplashAppStarter: app_ nulo em start()");
    return 1;
}

}  // namespace source
