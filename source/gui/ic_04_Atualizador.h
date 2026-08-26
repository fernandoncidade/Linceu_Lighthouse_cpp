#pragma once

namespace source::gui { class InterfaceMonitor; }

namespace source::gui {

class Atualizador {
public:
    static void atualizar_interface(InterfaceMonitor* interface_monitor);
    static void atualizar_status(InterfaceMonitor* interface_monitor);
    static void abrir_janela_filtros(InterfaceMonitor* interface_monitor);
};

}  // namespace source::gui
