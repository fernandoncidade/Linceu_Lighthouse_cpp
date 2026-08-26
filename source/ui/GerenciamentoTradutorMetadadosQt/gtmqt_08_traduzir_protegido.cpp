#include "gtmqt_08_traduzir_protegido.h"

#include <QStringList>

#include "gtmqt_06_traduzir_atributos.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/ui_13_TradutorMetadadosQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoTradutorMetadadosQt {

QString traduzir_protegido(source::ui::TradutorMetadadosQt* self, const QString& valor) {
    try {
        if (self == nullptr || self->loc_ == nullptr) {
            return valor;
        }

        const QStringList valores_sim = {
            QStringLiteral("sim"),
            QStringLiteral("yes"),
            QStringLiteral("sí"),
            QStringLiteral("oui"),
            QStringLiteral("sì"),
            QStringLiteral("ja"),
        };
        const QStringList valores_nao = {
            QStringLiteral("não"),
            QStringLiteral("no"),
            QStringLiteral("non"),
            QStringLiteral("nein"),
        };

        const QString valor_lower = valor.toLower();

        bool contem_sim = false;
        for (const QString& v : valores_sim) {
            if (valor_lower.contains(v)) {
                contem_sim = true;
                break;
            }
        }

        if (contem_sim) {
            QString valor_sim_usado;
            for (const QString& v : valores_sim) {
                if (valor_lower.startsWith(v)) {
                    valor_sim_usado = v;
                    break;
                }
            }

            if (!valor_sim_usado.isEmpty()) {
                QString resto = valor.mid(valor_sim_usado.size()).trimmed();
                if (!resto.isEmpty()) {
                    if (resto.startsWith(QStringLiteral(","))) {
                        resto = resto.mid(1).trimmed();
                    } else if (resto.startsWith(QStringLiteral("("))) {
                        return self->loc_->get_text(QStringLiteral("yes")) + resto;
                    }

                    if (!resto.isEmpty()) {
                        const QString resto_traduzido = traduzir_atributos(self, resto);
                        return self->loc_->get_text(QStringLiteral("yes")) + QStringLiteral(", ") +
                               resto_traduzido;
                    }
                }

                return self->loc_->get_text(QStringLiteral("yes"));
            }
        }

        for (const QString& v : valores_nao) {
            if (valor_lower.contains(v)) {
                return self->loc_->get_text(QStringLiteral("no"));
            }
        }

        return valor;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao traduzir protegido", true);
        return valor;
    }
}

}  // namespace source::ui::GerenciamentoTradutorMetadadosQt
