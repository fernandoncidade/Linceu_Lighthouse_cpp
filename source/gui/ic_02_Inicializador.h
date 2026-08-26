#pragma once

namespace source::gui { class InterfaceMonitor; }

namespace source::gui {

class Inicializador {
public:
    static void inicializar_componentes(InterfaceMonitor* interface_monitor);
    static void inicializar_gerenciadores(InterfaceMonitor* interface_monitor);
    static void inicializar_atributos(InterfaceMonitor* interface_monitor);
};

}  // namespace source::gui
