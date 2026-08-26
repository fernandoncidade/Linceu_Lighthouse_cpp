#include "gtab_01_DetectarTemaWindows.h"

#include <QSettings>

#include "source/services/GerenciamentoTabela/gtab_34_MonitorTemaWindows.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

QString detectar_tema_windows(source::services::GerenciadorTabela* self) {
    try {
        if (!self) return QStringLiteral("claro");

        if (auto* monitor = qobject_cast<MonitorTemaWindows*>(self->monitor_tema)) {
            const QString tema = monitor->obter_tema_atual();
            if (!tema.isEmpty()) return tema;
        }

        QSettings s(QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"),
                    QSettings::NativeFormat);
        const int v = s.value(QStringLiteral("AppsUseLightTheme"), 1).toInt();
        return v == 1 ? QStringLiteral("claro") : QStringLiteral("escuro");
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em detectar_tema_windows", true);
    }
    return QStringLiteral("claro");
}
}  // namespace source::services::GerenciamentoTabela

