#pragma once

#include <QChar>
#include <QHash>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QStringView>

#include "source/ui/ui_12_LocalizadorQt.h"

namespace source::services::GerenciamentoBaseEvento {

inline QString normalizar_chave_operacao(const QString& raw_key) {
    const QString decomposed = raw_key.trimmed().normalized(QString::NormalizationForm_D);
    QString cleaned;
    cleaned.reserve(decomposed.size());

    for (const QChar ch : decomposed) {
        const auto category = ch.category();
        if (category == QChar::Mark_NonSpacing || category == QChar::Mark_SpacingCombining ||
            category == QChar::Mark_Enclosing) {
            continue;
        }
        cleaned.append(ch);
    }

    return cleaned.toLower().simplified();
}

inline const QStringList& chaves_operacao_canonicas() {
    static const QStringList keys = {
        QStringLiteral("op_added"),
        QStringLiteral("op_deleted"),
        QStringLiteral("op_modified"),
        QStringLiteral("op_renamed"),
        QStringLiteral("op_moved"),
        QStringLiteral("op_scanned"),
    };
    return keys;
}

inline const QHash<QString, QStringList>& grupos_aliases_operacao() {
    static const QHash<QString, QStringList> groups = {
        {QStringLiteral("op_added"),
         {QStringLiteral("op_added"), QStringLiteral("added"), QStringLiteral("adicionado"),
          QStringLiteral("añadido"), QStringLiteral("ajouté"), QStringLiteral("aggiunto"),
          QStringLiteral("hinzugefügt")}},
        {QStringLiteral("op_deleted"),
         {QStringLiteral("op_deleted"), QStringLiteral("deleted"), QStringLiteral("excluido"),
          QStringLiteral("excluído"), QStringLiteral("eliminado"), QStringLiteral("eliminato"),
          QStringLiteral("supprimé"), QStringLiteral("gelöscht")}},
        {QStringLiteral("op_modified"),
         {QStringLiteral("op_modified"), QStringLiteral("modified"), QStringLiteral("modificado"),
          QStringLiteral("modifié"), QStringLiteral("modificato"), QStringLiteral("geändert")}},
        {QStringLiteral("op_renamed"),
         {QStringLiteral("op_renamed"), QStringLiteral("renamed"), QStringLiteral("renomeado"),
          QStringLiteral("renombrado"), QStringLiteral("renommé"), QStringLiteral("rinominato"),
          QStringLiteral("umbenannt")}},
        {QStringLiteral("op_moved"),
         {QStringLiteral("op_moved"), QStringLiteral("moved"), QStringLiteral("movido"),
          QStringLiteral("déplacé"), QStringLiteral("spostato"), QStringLiteral("verschoben")}},
        {QStringLiteral("op_scanned"),
         {QStringLiteral("op_scanned"), QStringLiteral("scanned"), QStringLiteral("escaneado"),
          QStringLiteral("escanneado"), QStringLiteral("numérisé"), QStringLiteral("scansionato"),
          QStringLiteral("gescannt"), QStringLiteral("scan")}},
    };
    return groups;
}

inline const QHash<QString, QString>& mapa_aliases_operacao_base() {
    static const QHash<QString, QString> map = []() {
        QHash<QString, QString> aliases;
        const auto& groups = grupos_aliases_operacao();
        for (auto it = groups.begin(); it != groups.end(); ++it) {
            for (const QString& alias : it.value()) {
                const QString normalized = normalizar_chave_operacao(alias);
                if (!normalized.isEmpty()) {
                    aliases.insert(normalized, it.key());
                }
            }
        }
        return aliases;
    }();
    return map;
}

inline QString chave_canonica_operacao(const QString& operation_key,
                                       source::ui::LocalizadorQt* loc = nullptr) {
    const QString normalized = normalizar_chave_operacao(operation_key);
    if (normalized.isEmpty()) {
        return {};
    }

    const QString base = mapa_aliases_operacao_base().value(normalized);
    if (!base.isEmpty()) {
        return base;
    }

    if (loc != nullptr) {
        for (const QString& key : chaves_operacao_canonicas()) {
            if (normalizar_chave_operacao(loc->get_text(key)) == normalized) {
                return key;
            }
        }
    }

    return {};
}

inline bool operacao_corresponde(const QString& operation_key,
                                 QStringView canonical_key,
                                 source::ui::LocalizadorQt* loc = nullptr) {
    return chave_canonica_operacao(operation_key, loc) == canonical_key.toString();
}

inline QString tabela_para_operacao(const QString& operation_key,
                                    source::ui::LocalizadorQt* loc = nullptr) {
    const QString canonical = chave_canonica_operacao(operation_key, loc);
    if (canonical == QStringLiteral("op_added")) {
        return QStringLiteral("adicionado");
    }
    if (canonical == QStringLiteral("op_deleted")) {
        return QStringLiteral("excluido");
    }
    if (canonical == QStringLiteral("op_modified")) {
        return QStringLiteral("modificado");
    }
    if (canonical == QStringLiteral("op_renamed")) {
        return QStringLiteral("renomeado");
    }
    if (canonical == QStringLiteral("op_moved")) {
        return QStringLiteral("movido");
    }
    if (canonical == QStringLiteral("op_scanned")) {
        return QStringLiteral("escaneado");
    }
    return {};
}

inline QStringList aliases_operacao(QStringView canonical_key,
                                    source::ui::LocalizadorQt* loc = nullptr) {
    const QString canonical = canonical_key.toString();
    QStringList aliases = grupos_aliases_operacao().value(canonical);
    if (loc != nullptr) {
        aliases.append(loc->get_text(canonical));
    }

    QStringList deduped;
    QSet<QString> seen;
    for (const QString& alias : aliases) {
        const QString trimmed = alias.trimmed();
        if (trimmed.isEmpty()) {
            continue;
        }
        const QString normalized = normalizar_chave_operacao(trimmed);
        if (normalized.isEmpty() || seen.contains(normalized)) {
            continue;
        }
        seen.insert(normalized);
        deduped.append(trimmed);
    }
    return deduped;
}

inline QString marcadores_sql(int count) {
    QStringList markers;
    markers.reserve(count);
    for (int i = 0; i < count; ++i) {
        markers.append(QStringLiteral("?"));
    }
    return markers.join(QLatin1Char(','));
}

}  // namespace source::services::GerenciamentoBaseEvento
