#include "secore_04_traduzir_strings_ui.h"

#include <QHash>
#include <QRegularExpression>
#include <QStringList>

#include "source/ui/ui_12_LocalizadorQt.h"

namespace source::security {
namespace {

QString humanizar_token(QString value) {
    value = value.trimmed();
    if (value.isEmpty()) {
        return value;
    }

    value.replace(QLatin1Char('_'), QLatin1Char(' '));
    return value.simplified();
}

QString traduzir_chave(source::ui::LocalizadorQt* loc,
                       const QString& key,
                       const QString& fallback = QString()) {
    const QString resolved_fallback = fallback.isEmpty() ? humanizar_token(key) : fallback;
    return loc != nullptr ? loc->get_text(key, resolved_fallback) : resolved_fallback;
}

QString traduzir_lista_por_ponto_virgula(source::ui::LocalizadorQt* loc, const QString& raw_value) {
    const QStringList parts = raw_value.split(QLatin1Char(';'), Qt::SkipEmptyParts);
    QStringList translated;
    translated.reserve(parts.size());

    for (const QString& part : parts) {
        const QString token = part.trimmed();
        if (!token.isEmpty()) {
            translated.append(traduzir_chave(loc, token));
        }
    }

    return translated.join(QStringLiteral("; "));
}

QString traduzir_baseline_status(source::ui::LocalizadorQt* loc, const QString& raw_value) {
    static const QHash<QString, QString> map = {
        {QStringLiteral("unavailable"), QStringLiteral("security_baseline_unavailable")},
        {QStringLiteral("directory"), QStringLiteral("security_baseline_directory")},
        {QStringLiteral("new"), QStringLiteral("security_baseline_new")},
        {QStringLiteral("unchanged"), QStringLiteral("security_baseline_unchanged")},
        {QStringLiteral("changed"), QStringLiteral("security_baseline_changed")},
        {QStringLiteral("missing"), QStringLiteral("security_baseline_missing")},
    };

    const QString translation_key = map.value(raw_value.trimmed().toLower());
    return translation_key.isEmpty() ? raw_value : traduzir_chave(loc, translation_key, raw_value);
}

QString traduzir_session_label(source::ui::LocalizadorQt* loc, const QString& raw_value) {
    return traduzir_chave(loc, raw_value, humanizar_token(raw_value));
}

QString traduzir_diff_summary(source::ui::LocalizadorQt* loc, const QString& raw_value) {
    const QString trimmed = raw_value.trimmed();
    if (trimmed.isEmpty()) {
        return trimmed;
    }

    if (trimmed == QStringLiteral("sem_alteracao_textual")) {
        return traduzir_chave(loc, QStringLiteral("sem_alteracao_textual"));
    }

    if (trimmed.startsWith(QStringLiteral("security_diff_lines_changed|"))) {
        const QStringList parts = trimmed.split(QLatin1Char('|'));
        if (parts.size() >= 3) {
            const QString pattern =
                traduzir_chave(loc,
                               QStringLiteral("security_diff_lines_changed"),
                               QStringLiteral("%1 lines added / %2 lines removed"));
            return pattern.arg(parts.at(1), parts.at(2));
        }
    }

    static const QRegularExpression legacy_pattern(
        QStringLiteral("^\\+(\\d+)\\s*/\\s*-(\\d+)\\s+linhas$"),
        QRegularExpression::CaseInsensitiveOption);
    const QRegularExpressionMatch match = legacy_pattern.match(trimmed);
    if (match.hasMatch()) {
        const QString pattern =
            traduzir_chave(loc,
                           QStringLiteral("security_diff_lines_changed"),
                           QStringLiteral("%1 lines added / %2 lines removed"));
        return pattern.arg(match.captured(1), match.captured(2));
    }

    return trimmed;
}

QString traduzir_risk_level(source::ui::LocalizadorQt* loc, const QString& raw_value) {
    static const QHash<QString, QString> map = {
        {QStringLiteral("none"), QStringLiteral("security_risk_none")},
        {QStringLiteral("low"), QStringLiteral("security_risk_low")},
        {QStringLiteral("medium"), QStringLiteral("security_risk_medium")},
        {QStringLiteral("high"), QStringLiteral("security_risk_high")},
        {QStringLiteral("critical"), QStringLiteral("security_risk_critical")},
    };

    const QString translation_key = map.value(raw_value.trimmed().toLower());
    return translation_key.isEmpty() ? raw_value : traduzir_chave(loc, translation_key, raw_value);
}

QString traduzir_quarantine_status(source::ui::LocalizadorQt* loc, const QString& raw_value) {
    static const QHash<QString, QString> map = {
        {QStringLiteral("not_applicable"), QStringLiteral("security_quarantine_not_applicable")},
        {QStringLiteral("queued"), QStringLiteral("security_quarantine_queued")},
        {QStringLiteral("captured"), QStringLiteral("security_quarantine_captured")},
        {QStringLiteral("copy_failed"), QStringLiteral("security_quarantine_copy_failed")},
    };

    const QString normalized = raw_value.trimmed().toLower();
    const QString translation_key = map.value(normalized);
    return translation_key.isEmpty() ? traduzir_chave(loc, normalized, raw_value)
                                     : traduzir_chave(loc, translation_key, raw_value);
}

QString traduzir_actor_process(source::ui::LocalizadorQt* loc, const QString& raw_value) {
    const QString normalized = raw_value.trimmed().toLower();
    if (normalized.isEmpty() ||
        normalized == QStringLiteral("unknown_process") ||
        normalized == QStringLiteral("desconhecido")) {
        return traduzir_chave(loc, QStringLiteral("unknown_process"));
    }
    return raw_value;
}

}  // namespace

bool is_ui_translatable_field(const QString& field_key) {
    static const QStringList fields = {
        QStringLiteral("baseline_status"),
        QStringLiteral("alerta_resumo"),
        QStringLiteral("session_label"),
        QStringLiteral("diff_resumo"),
        QStringLiteral("risk_level"),
        QStringLiteral("quarantine_status"),
        QStringLiteral("actor_process"),
        QStringLiteral("risk_reasons"),
        QStringLiteral("integrity_flags"),
    };
    return fields.contains(field_key);
}

QString traduzir_valor_ui(source::ui::LocalizadorQt* loc,
                          const QString& field_key,
                          const QString& raw_value) {
    if (raw_value.trimmed().isEmpty()) {
        return raw_value;
    }

    if (field_key == QStringLiteral("baseline_status")) {
        return traduzir_baseline_status(loc, raw_value);
    }
    if (field_key == QStringLiteral("alerta_resumo") ||
        field_key == QStringLiteral("risk_reasons") ||
        field_key == QStringLiteral("integrity_flags")) {
        return traduzir_lista_por_ponto_virgula(loc, raw_value);
    }
    if (field_key == QStringLiteral("session_label")) {
        return traduzir_session_label(loc, raw_value);
    }
    if (field_key == QStringLiteral("diff_resumo")) {
        return traduzir_diff_summary(loc, raw_value);
    }
    if (field_key == QStringLiteral("risk_level")) {
        return traduzir_risk_level(loc, raw_value);
    }
    if (field_key == QStringLiteral("quarantine_status")) {
        return traduzir_quarantine_status(loc, raw_value);
    }
    if (field_key == QStringLiteral("actor_process")) {
        return traduzir_actor_process(loc, raw_value);
    }

    return raw_value;
}

}  // namespace source::security
