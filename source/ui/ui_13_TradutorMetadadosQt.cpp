#include "ui_13_TradutorMetadadosQt.h"

#include "ui_12_LocalizadorQt.h"
#include "source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_01_verificar_idioma_cache.h"
#include "source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_02_traduzir_tipo_operacao.h"
#include "source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_03_obter_chave_traducao_reversa.h"
#include "source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_04_traduzir_metadados.h"
#include "source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_05_traduzir_tipo_arquivo.h"
#include "source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_06_traduzir_atributos.h"
#include "source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_07_traduzir_autor.h"
#include "source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_08_traduzir_protegido.h"
#include "source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_09_traduzir_dimensoes.h"
#include "source/utils/LogManager.h"

namespace source::ui {

TradutorMetadadosQt::TradutorMetadadosQt(LocalizadorQt* localizador) : loc_(localizador) {
    try {
        idioma_cache_ = (loc_ != nullptr) ? loc_->property("idioma_atual").toString() : QString();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar TradutorMetadadosQt", true);
    }
}

void TradutorMetadadosQt::verificar_idioma_cache() { GerenciamentoTradutorMetadadosQt::verificar_idioma_cache(this); }
QString TradutorMetadadosQt::traduzir_tipo_operacao(const QString& tipo_operacao, const QString& idioma_origem) { return GerenciamentoTradutorMetadadosQt::traduzir_tipo_operacao(this, tipo_operacao, idioma_origem); }
QString TradutorMetadadosQt::obter_chave_traducao_reversa(const QString& valor) const { return GerenciamentoTradutorMetadadosQt::obter_chave_traducao_reversa(const_cast<TradutorMetadadosQt*>(this), valor); }
QString TradutorMetadadosQt::traduzir_metadados(const QString& valor, const QString& campo) { return GerenciamentoTradutorMetadadosQt::traduzir_metadados(this, valor, campo); }
QVariantMap TradutorMetadadosQt::traduzir_metadados(const QVariantMap& metadados) {
    QVariantMap result = metadados;
    for (auto it = result.begin(); it != result.end(); ++it) {
        if (it.value().metaType().id() == QMetaType::QString) {
            it.value() = traduzir_metadados(it.value().toString(), it.key());
        }
    }
    return result;
}
QString TradutorMetadadosQt::traduzir_tipo_arquivo(const QString& tipo_arquivo) const { return GerenciamentoTradutorMetadadosQt::traduzir_tipo_arquivo(const_cast<TradutorMetadadosQt*>(this), tipo_arquivo); }
QString TradutorMetadadosQt::traduzir_atributos(const QString& atributos) const { return GerenciamentoTradutorMetadadosQt::traduzir_atributos(const_cast<TradutorMetadadosQt*>(this), atributos); }
QString TradutorMetadadosQt::traduzir_autor(const QString& autor) const { return GerenciamentoTradutorMetadadosQt::traduzir_autor(const_cast<TradutorMetadadosQt*>(this), autor); }
QString TradutorMetadadosQt::traduzir_protegido(const QString& protegido) const { return GerenciamentoTradutorMetadadosQt::traduzir_protegido(const_cast<TradutorMetadadosQt*>(this), protegido); }
QString TradutorMetadadosQt::traduzir_dimensoes(const QString& dimensoes) const { return GerenciamentoTradutorMetadadosQt::traduzir_dimensoes(const_cast<TradutorMetadadosQt*>(this), dimensoes); }

}  // namespace source::ui
