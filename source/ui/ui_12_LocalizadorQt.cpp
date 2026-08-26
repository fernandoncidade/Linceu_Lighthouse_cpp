#include "ui_12_LocalizadorQt.h"

#include <QCoreApplication>
#include <QDir>
#include <QLocale>
#include <QStringList>
#include <QTranslator>

#include "source/ui/GerenciamentoLocalizadorQt/glqt_01_inicializar_tradutor_metadados.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_02_criar_mapa_compatibilidade.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_03_carregar_tradutor.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_04_set_idioma.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_05_get_text.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_06_get_fallback_text.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_07_tr.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_08_salvar_preferencia_idioma.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_09_carregar_preferencia_idioma.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_10_get_idiomas_disponiveis.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_11_traduzir_tipo_operacao.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_12_traduzir_metadados.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_13_traduzir_tipo_operacao_fallback.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_14_criar_arquivos_traducao.h"
#include "source/utils/LogManager.h"

namespace source::ui {

LocalizadorQt::LocalizadorQt(QObject* parent)
    : QObject(parent), translator_(new QTranslator(this)), qt_translator_(new QTranslator(this)), tradutor_metadados_(nullptr) {
    try {
        idiomas_suportados_ = {{"pt_BR", "Português Brasileiro"}, {"en_US", "English (US)"},
                               {"es_ES", "Español"}, {"fr_FR", "Français"},
                               {"it_IT", "Italiano"}, {"de_DE", "Deutsch"}};

        system_locale_ = QLocale::system().name();
        idioma_atual_ = carregar_preferencia_idioma();

        const QString file_dir = QDir(QCoreApplication::applicationDirPath()).absolutePath();
        const QString parent_dir = QDir(file_dir + "/..").absolutePath();
        const QString grandparent_dir = QDir(parent_dir + "/..").absolutePath();

        const QStringList candidates = {
            QDir(file_dir).filePath("source/locale"),
            QDir(file_dir).filePath("locale"),
            QDir(parent_dir).filePath("source/locale"),
            QDir(parent_dir).filePath("locale"),
            QDir(grandparent_dir).filePath("source/locale"),
            QDir(grandparent_dir).filePath("locale")
        };

        translations_dir_.clear();
        for (const QString& candidate : candidates) {
            if (QDir(candidate).exists()) {
                translations_dir_ = QDir(candidate).absolutePath();
                break;
            }
        }
        if (translations_dir_.isEmpty()) {
            translations_dir_ = QDir(parent_dir).filePath("source/locale");
        }
        QDir().mkpath(translations_dir_);

        setProperty("translations_dir", translations_dir_);
        setProperty("idioma_atual", idioma_atual_);

        traducoes_ = criar_mapa_compatibilidade();
        inicializar_tradutor_metadados();
        carregar_tradutor(idioma_atual_);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar LocalizadorQt", true);
    }
}

void LocalizadorQt::inicializar_tradutor_metadados() { GerenciamentoLocalizadorQt::inicializar_tradutor_metadados(this); }
QVariantMap LocalizadorQt::criar_mapa_compatibilidade() const { return GerenciamentoLocalizadorQt::criar_mapa_compatibilidade(const_cast<LocalizadorQt*>(this)); }
void LocalizadorQt::carregar_tradutor(const QString& idioma) { (void)GerenciamentoLocalizadorQt::carregar_tradutor(this, idioma); }
void LocalizadorQt::set_idioma(const QString& idioma) { GerenciamentoLocalizadorQt::set_idioma(this, idioma); }
QString LocalizadorQt::get_text(const QString& key) const { return GerenciamentoLocalizadorQt::get_text(const_cast<LocalizadorQt*>(this), key); }
QString LocalizadorQt::get_text(const QString& key, const QString& fallback) const {
    const QString out = GerenciamentoLocalizadorQt::get_text(const_cast<LocalizadorQt*>(this), key);
    return out.isEmpty() ? fallback : out;
}
QString LocalizadorQt::get_text_args(const QString& key, const QStringList& args) const { return GerenciamentoLocalizadorQt::get_text(const_cast<LocalizadorQt*>(this), key, args); }
QString LocalizadorQt::get_fallback_text(const QString& key) const { return GerenciamentoLocalizadorQt::get_fallback_text(const_cast<LocalizadorQt*>(this), key); }
QString LocalizadorQt::tr(const QString& key) const { return GerenciamentoLocalizadorQt::tr(const_cast<LocalizadorQt*>(this), key); }
QString LocalizadorQt::tr_args(const QString& key, const QStringList& args) const { return GerenciamentoLocalizadorQt::tr(const_cast<LocalizadorQt*>(this), key, args); }
void LocalizadorQt::salvar_preferencia_idioma(const QString& idioma) { GerenciamentoLocalizadorQt::salvar_preferencia_idioma(this, idioma); }
QString LocalizadorQt::carregar_preferencia_idioma() const { return GerenciamentoLocalizadorQt::carregar_preferencia_idioma(const_cast<LocalizadorQt*>(this)); }
QVariantMap LocalizadorQt::get_idiomas_disponiveis() const {
    QVariantMap out;
    const auto idiomas = GerenciamentoLocalizadorQt::get_idiomas_disponiveis(const_cast<LocalizadorQt*>(this));
    for (auto it = idiomas.constBegin(); it != idiomas.constEnd(); ++it) out.insert(it.key(), it.value());
    return out;
}
QString LocalizadorQt::traduzir_tipo_operacao(const QString& tipo) const { return GerenciamentoLocalizadorQt::traduzir_tipo_operacao(const_cast<LocalizadorQt*>(this), tipo); }
QString LocalizadorQt::traduzir_metadados(const QString& meta) const { return GerenciamentoLocalizadorQt::traduzir_metadados(const_cast<LocalizadorQt*>(this), meta, QString()); }
QString LocalizadorQt::traduzir_tipo_operacao_fallback(const QString& tipo) const { return GerenciamentoLocalizadorQt::traduzir_tipo_operacao_fallback(const_cast<LocalizadorQt*>(this), tipo); }
void LocalizadorQt::criar_arquivos_traducao() { GerenciamentoLocalizadorQt::criar_arquivos_traducao(this); }

}  // namespace source::ui
