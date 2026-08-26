#ifndef SOURCE_UI_GERENCIAMENTOTRADUTORMETADADOSQT_GTMQT_04_TRADUZIR_METADADOS_H
#define SOURCE_UI_GERENCIAMENTOTRADUTORMETADADOSQT_GTMQT_04_TRADUZIR_METADADOS_H

#include <QString>

namespace source::ui { class TradutorMetadadosQt; }

namespace source::ui::GerenciamentoTradutorMetadadosQt {
QString traduzir_metadados(source::ui::TradutorMetadadosQt* self, const QString& valor, const QString& campo);
}

#endif // SOURCE_UI_GERENCIAMENTOTRADUTORMETADADOSQT_GTMQT_04_TRADUZIR_METADADOS_H
