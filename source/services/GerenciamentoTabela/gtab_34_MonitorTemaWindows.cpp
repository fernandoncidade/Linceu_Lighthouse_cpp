#include "gtab_34_MonitorTemaWindows.h"

#include <QSettings>

#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

MonitorTemaWindows::MonitorTemaWindows(QObject* parent)
    : QObject(parent), tema_atual_(detectar_tema_atual()), monitorando_(false) {
    QObject::connect(&timer_verificacao_, &QTimer::timeout, this, &MonitorTemaWindows::verificar_mudanca_tema);
}

QString MonitorTemaWindows::detectar_tema_atual() const {
    try {
        QSettings s("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
        const int v = s.value("AppsUseLightTheme", 1).toInt();
        return v == 1 ? QStringLiteral("claro") : QStringLiteral("escuro");
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao detectar tema do Windows", true);
        return QStringLiteral("claro");
    }
}

QString MonitorTemaWindows::obter_tema_atual() const { return tema_atual_; }

void MonitorTemaWindows::iniciar_monitoramento() {
    if (!monitorando_) {
        monitorando_ = true;
        timer_verificacao_.start(1000);
    }
}

void MonitorTemaWindows::parar_monitoramento() {
    if (monitorando_) {
        monitorando_ = false;
        timer_verificacao_.stop();
    }
}

void MonitorTemaWindows::verificar_mudanca_tema() {
    try {
        const QString tema = detectar_tema_atual();
        if (tema != tema_atual_) {
            tema_atual_ = tema;
            emit tema_alterado(tema_atual_);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao verificar mudança de tema", true);
    }
}

}  // namespace source::services::GerenciamentoTabela
