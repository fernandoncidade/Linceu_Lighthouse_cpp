#pragma once

namespace source::gui { class InterfaceMonitor; }

namespace source::gui {

class Configurador {
public:
    static void setup_ui(InterfaceMonitor* interface_monitor);
    static void setup_menu_bar(InterfaceMonitor* interface_monitor);
};

}  // namespace source::gui
