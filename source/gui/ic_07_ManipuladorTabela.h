#pragma once

#include <QVariantMap>

namespace source::gui { class InterfaceMonitor; }

namespace source::gui {

class ManipuladorTabela {
public:
    static void configurar_tabela(InterfaceMonitor* interface_monitor);
    static void atualizar_visibilidade_colunas(InterfaceMonitor* interface_monitor, bool atualizar_em_massa = false);
    static void processar_cores_assincronamente(InterfaceMonitor* interface_monitor, const QVariantMap& dados = {});
    static void aguardar_processamentos_pendentes(InterfaceMonitor* interface_monitor, int timeout = 5);
};

}  // namespace source::gui
