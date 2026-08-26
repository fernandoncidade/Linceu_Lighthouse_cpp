#pragma once

#include <QHash>
#include <QString>
#include <QVariant>

namespace source::ui {

class LocalizadorQt;
class TradutorMetadadosQt;

namespace GerenciamentoTradutorMetadadosQt {
void verificar_idioma_cache(source::ui::TradutorMetadadosQt* self);
QString traduzir_tipo_operacao(source::ui::TradutorMetadadosQt* self, const QString& valor, const QString& idioma_origem = QString());
QString obter_chave_traducao_reversa(source::ui::TradutorMetadadosQt* self, const QString& valor);
QString traduzir_metadados(source::ui::TradutorMetadadosQt* self, const QString& valor, const QString& campo);
QString traduzir_tipo_arquivo(source::ui::TradutorMetadadosQt* self, const QString& valor);
QString traduzir_atributos(source::ui::TradutorMetadadosQt* self, const QString& valor);
QString traduzir_autor(source::ui::TradutorMetadadosQt* self, const QString& valor);
QString traduzir_protegido(source::ui::TradutorMetadadosQt* self, const QString& valor);
QString traduzir_dimensoes(source::ui::TradutorMetadadosQt* self, const QString& valor);
}

class TradutorMetadadosQt {
public:
    explicit TradutorMetadadosQt(LocalizadorQt* localizador);

    QString traduzir_tipo_operacao(const QString& tipo_operacao, const QString& idioma_origem = QString());
    QString traduzir_metadados(const QString& valor, const QString& campo);
    QVariantMap traduzir_metadados(const QVariantMap& metadados);

private:
    friend void source::ui::GerenciamentoTradutorMetadadosQt::verificar_idioma_cache(source::ui::TradutorMetadadosQt* self);
    friend QString source::ui::GerenciamentoTradutorMetadadosQt::traduzir_tipo_operacao(source::ui::TradutorMetadadosQt* self, const QString& valor, const QString& idioma_origem);
    friend QString source::ui::GerenciamentoTradutorMetadadosQt::obter_chave_traducao_reversa(source::ui::TradutorMetadadosQt* self, const QString& valor);
    friend QString source::ui::GerenciamentoTradutorMetadadosQt::traduzir_metadados(source::ui::TradutorMetadadosQt* self, const QString& valor, const QString& campo);
    friend QString source::ui::GerenciamentoTradutorMetadadosQt::traduzir_tipo_arquivo(source::ui::TradutorMetadadosQt* self, const QString& valor);
    friend QString source::ui::GerenciamentoTradutorMetadadosQt::traduzir_atributos(source::ui::TradutorMetadadosQt* self, const QString& valor);
    friend QString source::ui::GerenciamentoTradutorMetadadosQt::traduzir_autor(source::ui::TradutorMetadadosQt* self, const QString& valor);
    friend QString source::ui::GerenciamentoTradutorMetadadosQt::traduzir_protegido(source::ui::TradutorMetadadosQt* self, const QString& valor);
    friend QString source::ui::GerenciamentoTradutorMetadadosQt::traduzir_dimensoes(source::ui::TradutorMetadadosQt* self, const QString& valor);

    void verificar_idioma_cache();
    QString obter_chave_traducao_reversa(const QString& valor) const;
    QString traduzir_tipo_arquivo(const QString& tipo_arquivo) const;
    QString traduzir_atributos(const QString& atributos) const;
    QString traduzir_autor(const QString& autor) const;
    QString traduzir_protegido(const QString& protegido) const;
    QString traduzir_dimensoes(const QString& dimensoes) const;

private:
    LocalizadorQt* loc_;
    QHash<QString, QString> cache_traducoes_tipo_operacao_;
    QHash<QString, QString> cache_traducoes_tipo_;
    QHash<QString, QString> cache_traducoes_atributos_;
    QHash<QString, QString> cache_traducoes_protegido_;
    QString idioma_cache_;
};

}  // namespace source::ui
