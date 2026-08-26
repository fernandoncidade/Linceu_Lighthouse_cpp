#pragma once

#include <QString>

class QApplication;

namespace source::gui {

class InterfaceMonitor;

class Internacionalizador {
public:
    static bool inicializar_sistema_traducao(QApplication* app, const QString& idioma_inicial = QStringLiteral("pt_BR"));
    static void atualizar_tradutor_qt(InterfaceMonitor* interface_monitor, const QString& idioma);
    static bool inicializar_tradutor_qt(QApplication* app, const QString& idioma);
    static void extrair_strings_para_traducao();
    static void compilar_traducoes();
};

}  // namespace source::gui
