#pragma once

#include <QCheckBox>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QVariantMap>

#include "source/tools/fil_01_Filtros.h"
#include "source/ui/ui_12_LocalizadorQt.h"

namespace source::tools::GerenciamentoAdministradorFiltros::detail {

inline QString normalizar_operacao(const QString& valor) {
    QString normalizado = valor.trimmed().simplified().toCaseFolded();
    normalizado = normalizado.normalized(QString::NormalizationForm_D);

    QString sem_marcas;
    sem_marcas.reserve(normalizado.size());
    for (const QChar ch : normalizado) {
        const QChar::Category category = ch.category();
        if (category != QChar::Mark_NonSpacing &&
            category != QChar::Mark_SpacingCombining &&
            category != QChar::Mark_Enclosing) {
            sem_marcas.append(ch);
        }
    }
    return sem_marcas;
}

inline QStringList chaves_operacao() {
    return {
        QStringLiteral("op_moved"),
        QStringLiteral("op_renamed"),
        QStringLiteral("op_added"),
        QStringLiteral("op_deleted"),
        QStringLiteral("op_modified"),
        QStringLiteral("op_scanned"),
    };
}

inline const QHash<QString, QStringList>& aliases_operacao() {
    static const QHash<QString, QStringList> aliases = {
        {QStringLiteral("op_moved"),
         {QStringLiteral("moved"), QStringLiteral("movido"), QStringLiteral("move"),
          QStringLiteral("mover"), QStringLiteral("deslocado"), QStringLiteral("deplace"),
          QStringLiteral("spostato"), QStringLiteral("verschoben")}},
        {QStringLiteral("op_renamed"),
         {QStringLiteral("renamed"), QStringLiteral("renomeado"), QStringLiteral("renombrado"),
          QStringLiteral("renomme"), QStringLiteral("rinominato"), QStringLiteral("umbenannt")}},
        {QStringLiteral("op_added"),
         {QStringLiteral("added"), QStringLiteral("adicionado"), QStringLiteral("anadido"),
          QStringLiteral("agregado"), QStringLiteral("ajoute"), QStringLiteral("aggiunto"),
          QStringLiteral("hinzugefugt")}},
        {QStringLiteral("op_deleted"),
         {QStringLiteral("deleted"), QStringLiteral("excluido"), QStringLiteral("removido"),
          QStringLiteral("eliminado"), QStringLiteral("supprime"), QStringLiteral("geloscht")}},
        {QStringLiteral("op_modified"),
         {QStringLiteral("modified"), QStringLiteral("modificado"), QStringLiteral("modifie"),
          QStringLiteral("modificato"), QStringLiteral("geandert")}},
        {QStringLiteral("op_scanned"),
         {QStringLiteral("scanned"), QStringLiteral("escaneado"), QStringLiteral("digitalizado"),
          QStringLiteral("scan"), QStringLiteral("scansionato"), QStringLiteral("gescannt"),
          QStringLiteral("numerise")}},
    };
    return aliases;
}

inline QString resolver_chave_operacao(source::ui::LocalizadorQt* loc, const QString& valor) {
    const QString normalizado = normalizar_operacao(valor);
    if (normalizado.isEmpty()) {
        return {};
    }

    for (const QString& chave : chaves_operacao()) {
        if (normalizado == normalizar_operacao(chave)) {
            return chave;
        }
        if (loc != nullptr && normalizado == normalizar_operacao(loc->get_text(chave))) {
            return chave;
        }
    }

    const auto& aliases = aliases_operacao();
    for (auto it = aliases.constBegin(); it != aliases.constEnd(); ++it) {
        for (const QString& alias : it.value()) {
            if (normalizado == normalizar_operacao(alias)) {
                return it.key();
            }
        }
    }

    return {};
}

inline QCheckBox* checkbox_operacao(source::tools::Filtros* parent, const QString& chave_operacao) {
    if (parent == nullptr || chave_operacao.isEmpty()) {
        return nullptr;
    }

    const QVariantMap map = parent->property("checkboxes_operacao").toMap();
    return qobject_cast<QCheckBox*>(map.value(chave_operacao).value<QObject*>());
}

inline bool operacao_permitida(source::tools::Filtros* parent, const QString& tipo_operacao) {
    if (parent == nullptr) {
        return true;
    }

    const QString chave = resolver_chave_operacao(parent->localizador(), tipo_operacao);
    if (chave.isEmpty()) {
        return true;
    }

    QCheckBox* checkbox = checkbox_operacao(parent, chave);
    return checkbox == nullptr || checkbox->isChecked();
}

}  // namespace source::tools::GerenciamentoAdministradorFiltros::detail
