#pragma once

namespace source::gui { class InterfaceMonitor; }

namespace source::gui {

class GerenciadorProgresso {
public:
    static void criar_barra_progresso(InterfaceMonitor* interface_monitor);
    static void atualizar_progresso_scan(InterfaceMonitor* interface_monitor, int progresso, int contador, int total);
    static void esconder_barra_progresso(InterfaceMonitor* interface_monitor);
};

}  // namespace source::gui
