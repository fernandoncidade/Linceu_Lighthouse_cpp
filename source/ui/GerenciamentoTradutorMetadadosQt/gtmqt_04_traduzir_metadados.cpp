#include "gtmqt_04_traduzir_metadados.h"

#include "gtmqt_01_verificar_idioma_cache.h"
#include "gtmqt_05_traduzir_tipo_arquivo.h"
#include "gtmqt_06_traduzir_atributos.h"
#include "gtmqt_07_traduzir_autor.h"
#include "gtmqt_08_traduzir_protegido.h"
#include "gtmqt_09_traduzir_dimensoes.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/ui_13_TradutorMetadadosQt.h"
#include "source/utils/LogManager.h"

#include <QSet>
#include <QStringList>

namespace source::ui::GerenciamentoTradutorMetadadosQt {
namespace {

QString normalizar_chave_mime(QString value) {
    value = value.trimmed().toLower();
    QString out;
    out.reserve(value.size());
    bool last_underscore = false;
    for (const QChar ch : value) {
        if (ch.isLetterOrNumber()) {
            out.append(ch);
            last_underscore = false;
        } else if (!last_underscore) {
            out.append(QLatin1Char('_'));
            last_underscore = true;
        }
    }
    while (out.startsWith(QLatin1Char('_'))) out.remove(0, 1);
    while (out.endsWith(QLatin1Char('_'))) out.chop(1);
    return out;
}

QString humanizar_subtipo_mime(QString value) {
    value = value.trimmed();
    if (value.isEmpty()) {
        return value;
    }

    QString normalized = value.toLower();
    if (normalized.startsWith(QStringLiteral("x-"))) {
        normalized = normalized.mid(2);
    }
    if (normalized.startsWith(QStringLiteral("vnd."))) {
        normalized = normalized.mid(4);
    }

    normalized.replace(QLatin1Char('+'), QLatin1Char(' '));
    normalized.replace(QLatin1Char('-'), QLatin1Char(' '));
    normalized.replace(QLatin1Char('.'), QLatin1Char(' '));
    normalized.replace(QLatin1Char('_'), QLatin1Char(' '));

    static const QSet<QString> acronimos = {
        QStringLiteral("7z"), QStringLiteral("avi"), QStringLiteral("bmp"), QStringLiteral("csv"),
        QStringLiteral("epub"), QStringLiteral("gif"), QStringLiteral("gzip"), QStringLiteral("html"),
        QStringLiteral("jpeg"), QStringLiteral("jpg"), QStringLiteral("json"), QStringLiteral("mp3"),
        QStringLiteral("mp4"), QStringLiteral("mpeg"), QStringLiteral("ogg"), QStringLiteral("pdf"),
        QStringLiteral("png"), QStringLiteral("rar"), QStringLiteral("rtf"), QStringLiteral("svg"),
        QStringLiteral("tiff"), QStringLiteral("wav"), QStringLiteral("webm"), QStringLiteral("webp"),
        QStringLiteral("xhtml"), QStringLiteral("xml"), QStringLiteral("zip")
    };

    QStringList words;
    const QStringList tokens = normalized.simplified().split(QLatin1Char(' '), Qt::SkipEmptyParts);
    words.reserve(tokens.size());
    for (const QString& token : tokens) {
        if (acronimos.contains(token)) {
            words.append(token.toUpper());
        } else {
            words.append(token.left(1).toUpper() + token.mid(1));
        }
    }
    return words.join(QLatin1Char(' '));
}

QString traduzir_chave(source::ui::LocalizadorQt* loc, const QString& key, const QString& fallback) {
    if (loc == nullptr) {
        return fallback;
    }

    const QString translated = loc->get_text(key);
    return translated.isEmpty() || translated == key ? fallback : translated;
}

QString traduzir_mime(source::ui::LocalizadorQt* loc, const QString& valor) {
    const QString raw = valor.trimmed();
    if (raw.isEmpty()) {
        return raw;
    }

    const QStringList parts = raw.split(QLatin1Char('/'), Qt::SkipEmptyParts);
    const QString primary = parts.value(0).trimmed().toLower();
    const QString subtype = parts.size() > 1 ? parts.mid(1).join(QLatin1Char('/')).trimmed()
                                             : QString();

    const QString primary_key = QStringLiteral("mime_primary_") + normalizar_chave_mime(primary);
    const QString primary_fallback =
        primary.isEmpty()
            ? traduzir_chave(loc, QStringLiteral("mime_primary_other"), QStringLiteral("Other"))
            : humanizar_subtipo_mime(primary);
    const QString primary_text = traduzir_chave(loc, primary_key, primary_fallback);

    if (subtype.isEmpty()) {
        return QStringLiteral("%1 (%2)").arg(primary_text, raw);
    }

    const QString subtype_key = QStringLiteral("mime_subtype_") + normalizar_chave_mime(subtype);
    const QString subtype_text = traduzir_chave(loc, subtype_key, humanizar_subtipo_mime(subtype));
    return QStringLiteral("%1: %2 (%3)").arg(primary_text, subtype_text, raw);
}

}  // namespace

QString traduzir_metadados(source::ui::TradutorMetadadosQt* self, const QString& valor,
                           const QString& campo) {
    try {
        if (self == nullptr || self->loc_ == nullptr) {
            return valor;
        }

        verificar_idioma_cache(self);
        if (valor.isEmpty()) {
            return valor;
        }

        QString campo_lower = campo.toLower();
        const QString cache_key =
            valor + QStringLiteral("_") + campo_lower + QStringLiteral("_") +
            self->loc_->property("idioma_atual").toString();

        if (campo_lower == QStringLiteral("tipo") || campo_lower == QStringLiteral("type")) {
            if (self->cache_traducoes_tipo_.contains(cache_key)) {
                return self->cache_traducoes_tipo_.value(cache_key);
            }

            const QString resultado = traduzir_tipo_arquivo(self, valor);
            self->cache_traducoes_tipo_.insert(cache_key, resultado);
            return resultado;
        }

        if (campo_lower == QStringLiteral("mime")) {
            if (self->cache_traducoes_tipo_.contains(cache_key)) {
                return self->cache_traducoes_tipo_.value(cache_key);
            }

            const QString resultado = traduzir_mime(self->loc_, valor);
            self->cache_traducoes_tipo_.insert(cache_key, resultado);
            return resultado;
        }

        if (campo_lower == QStringLiteral("atributos") || campo_lower == QStringLiteral("attributes")) {
            if (self->cache_traducoes_atributos_.contains(cache_key)) {
                return self->cache_traducoes_atributos_.value(cache_key);
            }

            const QString resultado = traduzir_atributos(self, valor);
            self->cache_traducoes_atributos_.insert(cache_key, resultado);
            return resultado;
        }

        if (campo_lower == QStringLiteral("protegido") || campo_lower == QStringLiteral("protected")) {
            if (self->cache_traducoes_protegido_.contains(cache_key)) {
                return self->cache_traducoes_protegido_.value(cache_key);
            }

            const QString resultado = traduzir_protegido(self, valor);
            self->cache_traducoes_protegido_.insert(cache_key, resultado);
            return resultado;
        }

        campo_lower = campo.toLower();
        if (campo_lower == QStringLiteral("autor") || campo_lower == QStringLiteral("author")) {
            return traduzir_autor(self, valor);
        }

        if (campo_lower == QStringLiteral("dimensoes") || campo_lower == QStringLiteral("dimensions") ||
            campo_lower == QStringLiteral("tamanho") || campo_lower == QStringLiteral("size")) {
            return traduzir_dimensoes(self, valor);
        }

        return valor;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao traduzir metadados", true);
        return QString();
    }
}

}  // namespace source::ui::GerenciamentoTradutorMetadadosQt
