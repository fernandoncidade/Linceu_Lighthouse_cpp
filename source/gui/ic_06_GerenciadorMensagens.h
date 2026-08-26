#pragma once

#include <QString>

namespace source::gui { class InterfaceMonitor; }

namespace source::gui {

class GerenciadorMensagens {
public:
    static void atualizar_rotulos(InterfaceMonitor* interface_monitor);
    static void notificar_erro(InterfaceMonitor* interface_monitor, const QString& mensagem);
    static void notificar_sucesso(InterfaceMonitor* interface_monitor, const QString& mensagem);
};

}  // namespace source::gui
