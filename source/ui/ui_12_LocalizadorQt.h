#pragma once

#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class QTranslator;

namespace source::ui {

class LocalizadorQt;
class TradutorMetadadosQt;

namespace GerenciamentoLocalizadorQt {
void inicializar_tradutor_metadados(source::ui::LocalizadorQt* self);
QVariantMap criar_mapa_compatibilidade(source::ui::LocalizadorQt* self);
bool carregar_tradutor(source::ui::LocalizadorQt* self, const QString& idioma);
void set_idioma(source::ui::LocalizadorQt* self, const QString& idioma);
QString get_text(source::ui::LocalizadorQt* self, const QString& key, const QStringList& args = QStringList());
QString get_fallback_text(source::ui::LocalizadorQt* self, const QString& key);
QString tr(source::ui::LocalizadorQt* self, const QString& key, const QStringList& args = QStringList());
void salvar_preferencia_idioma(source::ui::LocalizadorQt* self, const QString& idioma);
QString carregar_preferencia_idioma(source::ui::LocalizadorQt* self);
QHash<QString, QString> get_idiomas_disponiveis(source::ui::LocalizadorQt* self);
QString traduzir_tipo_operacao(source::ui::LocalizadorQt* self, const QString& valor, const QString& idioma_origem = QString());
QString traduzir_metadados(source::ui::LocalizadorQt* self, const QString& valor, const QString& campo);
QString traduzir_tipo_operacao_fallback(source::ui::LocalizadorQt* self, const QString& valor);
void criar_arquivos_traducao(source::ui::LocalizadorQt* self);
}

class LocalizadorQt : public QObject {
    Q_OBJECT

public:
    explicit LocalizadorQt(QObject* parent = nullptr);

    void carregar_tradutor(const QString& idioma);
    Q_INVOKABLE void set_idioma(const QString& idioma);
    Q_INVOKABLE QString get_text(const QString& key) const;
    Q_INVOKABLE QString get_text(const QString& key, const QString& fallback) const;
    QString get_text_args(const QString& key, const QStringList& args) const;
    QString get_fallback_text(const QString& key) const;
    QString tr(const QString& key) const;
    QString tr_args(const QString& key, const QStringList& args) const;
    void salvar_preferencia_idioma(const QString& idioma);
    QString carregar_preferencia_idioma() const;
    Q_INVOKABLE QVariantMap get_idiomas_disponiveis() const;
    QString traduzir_tipo_operacao(const QString& tipo) const;
    QString traduzir_metadados(const QString& meta) const;
    QString traduzir_tipo_operacao_fallback(const QString& tipo) const;
    void criar_arquivos_traducao();

signals:
    void idioma_alterado(const QString& idioma);
    void traducoes_carregadas(const QString& idioma);

private:
    friend void source::ui::GerenciamentoLocalizadorQt::inicializar_tradutor_metadados(source::ui::LocalizadorQt* self);
    friend QVariantMap source::ui::GerenciamentoLocalizadorQt::criar_mapa_compatibilidade(source::ui::LocalizadorQt* self);
    friend bool source::ui::GerenciamentoLocalizadorQt::carregar_tradutor(source::ui::LocalizadorQt* self, const QString& idioma);
    friend void source::ui::GerenciamentoLocalizadorQt::set_idioma(source::ui::LocalizadorQt* self, const QString& idioma);
    friend QString source::ui::GerenciamentoLocalizadorQt::get_text(source::ui::LocalizadorQt* self, const QString& key, const QStringList& args);
    friend QString source::ui::GerenciamentoLocalizadorQt::get_fallback_text(source::ui::LocalizadorQt* self, const QString& key);
    friend QString source::ui::GerenciamentoLocalizadorQt::tr(source::ui::LocalizadorQt* self, const QString& key, const QStringList& args);
    friend void source::ui::GerenciamentoLocalizadorQt::salvar_preferencia_idioma(source::ui::LocalizadorQt* self, const QString& idioma);
    friend QString source::ui::GerenciamentoLocalizadorQt::carregar_preferencia_idioma(source::ui::LocalizadorQt* self);
    friend QHash<QString, QString> source::ui::GerenciamentoLocalizadorQt::get_idiomas_disponiveis(source::ui::LocalizadorQt* self);
    friend QString source::ui::GerenciamentoLocalizadorQt::traduzir_tipo_operacao(source::ui::LocalizadorQt* self, const QString& valor, const QString& idioma_origem);
    friend QString source::ui::GerenciamentoLocalizadorQt::traduzir_metadados(source::ui::LocalizadorQt* self, const QString& valor, const QString& campo);
    friend QString source::ui::GerenciamentoLocalizadorQt::traduzir_tipo_operacao_fallback(source::ui::LocalizadorQt* self, const QString& valor);
    friend void source::ui::GerenciamentoLocalizadorQt::criar_arquivos_traducao(source::ui::LocalizadorQt* self);

    void inicializar_tradutor_metadados();
    QVariantMap criar_mapa_compatibilidade() const;

private:
    QHash<QString, QString> idiomas_suportados_;
    QString system_locale_;
    QString idioma_atual_;
    QTranslator* translator_;
    QTranslator* qt_translator_;
    QString translations_dir_;
    QVariantMap traducoes_;
    source::ui::TradutorMetadadosQt* tradutor_metadados_;
};

}  // namespace source::ui
