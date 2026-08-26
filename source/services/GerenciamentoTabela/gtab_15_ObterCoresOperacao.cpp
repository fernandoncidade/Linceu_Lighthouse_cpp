#include "gtab_15_ObterCoresOperacao.h"

#include <QColor>
#include <QHash>
#include <QMetaObject>
#include <QStringList>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/ui/GerenciamentoMenusUI/gmui_02_GerenciadorCores.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

namespace {
QVariantMap to_map(const QHash<QString, QVariant>& hash) {
    QVariantMap out;
    for (auto it = hash.constBegin(); it != hash.constEnd(); ++it) out.insert(it.key(), it.value());
    return out;
}

QString normalizar_operacao(QString valor) {
    valor = valor.normalized(QString::NormalizationForm_D).toCaseFolded().trimmed();
    QString out;
    out.reserve(valor.size());
    for (const QChar ch : valor) {
        if (ch.category() == QChar::Mark_NonSpacing ||
            ch.category() == QChar::Mark_SpacingCombining ||
            ch.category() == QChar::Mark_Enclosing) {
            continue;
        }
        out.append(ch);
    }
    return out.simplified();
}

QString gt(QObject* loc, const QString& key, const QString& fallback) {
    QString out;
    if (loc && QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, out), Q_ARG(QString, key)) && !out.isEmpty()) {
        return out;
    }
    return fallback;
}

void inserir_chave_cor(QHash<QString, QVariant>& cores, const QString& chave, const QColor& cor) {
    if (chave.isEmpty()) {
        return;
    }
    cores.insert(chave, cor);
    cores.insert(chave.toCaseFolded(), cor);
    cores.insert(normalizar_operacao(chave), cor);
}

QStringList aliases_operacao(const QString& op_key) {
    if (op_key == QStringLiteral("op_renamed")) {
        return {QStringLiteral("renamed"), QStringLiteral("renomeado"), QStringLiteral("renombrado"),
                QStringLiteral("renomme"), QStringLiteral("rinominato"), QStringLiteral("umbenannt")};
    }
    if (op_key == QStringLiteral("op_added")) {
        return {QStringLiteral("added"), QStringLiteral("adicionado"), QStringLiteral("agregado"),
                QStringLiteral("ajoute"), QStringLiteral("aggiunto"), QStringLiteral("hinzugefugt")};
    }
    if (op_key == QStringLiteral("op_deleted")) {
        return {QStringLiteral("deleted"), QStringLiteral("excluido"), QStringLiteral("excluido"),
                QStringLiteral("eliminado"), QStringLiteral("supprime"), QStringLiteral("geloscht")};
    }
    if (op_key == QStringLiteral("op_modified")) {
        return {QStringLiteral("modified"), QStringLiteral("modificado"), QStringLiteral("modifie"),
                QStringLiteral("modificato"), QStringLiteral("geandert")};
    }
    if (op_key == QStringLiteral("op_moved")) {
        return {QStringLiteral("moved"), QStringLiteral("movido"), QStringLiteral("deplace"),
                QStringLiteral("spostato"), QStringLiteral("verschoben")};
    }
    if (op_key == QStringLiteral("op_scanned")) {
        return {QStringLiteral("scanned"), QStringLiteral("escaneado"), QStringLiteral("escanne"),
                QStringLiteral("scansionato"), QStringLiteral("gescannt"), QStringLiteral("scan")};
    }
    return {};
}
}

QVariantMap obter_cores_operacao(source::services::GerenciadorTabela* self) {
    try {
        if (!self) return QVariantMap{};
        if (!self->cache_cores.isEmpty()) return to_map(self->cache_cores);

        auto* gerenciador_cores = qobject_cast<source::ui::GerenciamentoMenusUI::GerenciadorCores*>(
            self->interface->property("gerenciador_cores").value<QObject*>());

        struct CorOperacaoPadrao {
            const char* key;
            QColor cor_padrao;
        };

        const CorOperacaoPadrao operacoes[] = {
            {"op_renamed", QColor(0, 255, 0)},
            {"op_added", QColor(0, 0, 255)},
            {"op_deleted", QColor(255, 0, 0)},
            {"op_modified", QColor(255, 98, 0)},
            {"op_moved", QColor(255, 0, 255)},
            {"op_scanned", QColor(128, 128, 128)}
        };

        QHash<QString, QVariant> cores;
        for (const auto& op : operacoes) {
            const QString op_key = QString::fromLatin1(op.key);
            const QString label = gt(self->loc, op_key, op_key);

            QColor cor_final = op.cor_padrao;
            if (gerenciador_cores != nullptr) {
                const QColor cor_configurada(gerenciador_cores->obter_cor_hex(op_key));
                if (cor_configurada.isValid()) {
                    cor_final = cor_configurada;
                }
            }

            inserir_chave_cor(cores, op_key, cor_final);
            inserir_chave_cor(cores, label, cor_final);
            const QStringList aliases = aliases_operacao(op_key);
            for (const QString& alias : aliases) {
                inserir_chave_cor(cores, alias, cor_final);
            }
        }

        self->cache_cores = cores;
        return to_map(cores);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em obter_cores_operacao", true);
    }
    return QVariantMap{};
}
}  // namespace source::services::GerenciamentoTabela
