#ifndef SOURCE_UI_GERENCIAMENTOTRADUTORMETADADOSQT_GTMQT_02_TRADUZIR_TIPO_OPERACAO_H
#define SOURCE_UI_GERENCIAMENTOTRADUTORMETADADOSQT_GTMQT_02_TRADUZIR_TIPO_OPERACAO_H

#include <QString>

namespace source::ui { class TradutorMetadadosQt; }

namespace source::ui::GerenciamentoTradutorMetadadosQt {
QString traduzir_tipo_operacao(source::ui::TradutorMetadadosQt* self, const QString& tipo_operacao, const QString& idioma_origem);
}

#endif // SOURCE_UI_GERENCIAMENTOTRADUTORMETADADOSQT_GTMQT_02_TRADUZIR_TIPO_OPERACAO_H
