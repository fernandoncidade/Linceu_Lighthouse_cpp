#include "gevmov_07_processar_exclusoes_pendentes.h"

#include <QDateTime>
#include <QTimer>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/services/ob_08_EventoMovido.h"
#include "source/utils/LogManager.h"

namespace {
bool ext_lenta(const QString& nome) {
    static const QStringList exts = {".heic", ".xlsx", ".xls", ".docx", ".pdf", ".mp4", ".mkv", ".mov", ".avi", ".xml", ".exe"};
    const QString n = nome.toLower();
    for (const QString& ext : exts) if (n.endsWith(ext)) return true;
    return false;
}

QList<QVariantMap> registros(const QVariantList& in) {
    QList<QVariantMap> out;
    for (const QVariant& v : in) out.append(v.toMap());
    return out;
}

QVariantList to_vlist(const QList<QVariantMap>& in) {
    QVariantList out;
    for (const auto& m : in) out.append(m);
    return out;
}

source::services::BaseEvento* resolver_evento_base(QObject* interfaceMonitor) {
    if (!interfaceMonitor) return nullptr;
    if (!interfaceMonitor->property("_aceitar_eventos_monitoramento").toBool() ||
        interfaceMonitor->property("_encerrando_interface").toBool()) {
        return nullptr;
    }

    auto* evento_base = interfaceMonitor->property("evento_base").value<source::services::BaseEvento*>();
    if (evento_base) return evento_base;

    const qulonglong obs_ptr = interfaceMonitor->property("_observador_cpp_ptr").toULongLong();
    if (obs_ptr != 0) {
        auto* obs = reinterpret_cast<source::services::Observador*>(obs_ptr);
        if (obs && obs->evento_base) return obs->evento_base.get();
    }
    return nullptr;
}
}

namespace source::services::GerenciamentoEventoMovido {

void processar_exclusoes_pendentes(QObject* interfaceMonitor) {
    try {
        if (!interfaceMonitor) return;

        QVariantMap excluidos = interfaceMonitor->property("excluidos_recentemente").toMap();
        QList<QVariantMap> eventos_para_processar;
        const double agora = QDateTime::currentMSecsSinceEpoch() / 1000.0;

        QStringList nomes_para_remover;
        for (auto it = excluidos.begin(); it != excluidos.end(); ++it) {
            const QString nome = it.key();
            QList<QVariantMap> lista = registros(it.value().toList());
            QList<QVariantMap> remanescentes;

            for (int i = 0; i < lista.size(); ++i) {
                const QVariantMap registro = lista.at(i);
                const double ts = registro.value("timestamp").toDouble();
                const double tempo_limite = ext_lenta(nome) ? 5.0 : 0.5;

                if ((agora - ts) > tempo_limite) {
                    QVariantMap novo_evento = registro.value("evento").toMap();
                    novo_evento.remove("_temporario");
                    if (!novo_evento.contains("timestamp") ||
                        novo_evento.value("timestamp").toString().trimmed().isEmpty()) {
                        novo_evento["timestamp"] =
                            QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(ts * 1000.0))
                                .toString("yyyy-MM-dd HH:mm:ss.zzz");
                    }
                    novo_evento["id_exclusao_unico"] = QString("%1_%2_%3").arg(nome).arg(ts).arg(i);
                    eventos_para_processar.append(novo_evento);
                } else {
                    remanescentes.append(registro);
                }
            }

            if (remanescentes.isEmpty()) nomes_para_remover.append(nome);
            else excluidos[nome] = to_vlist(remanescentes);
        }

        for (const QString& n : nomes_para_remover) excluidos.remove(n);
        interfaceMonitor->setProperty("excluidos_recentemente", excluidos);

        if (eventos_para_processar.isEmpty()) return;

        if (auto* evento_base = resolver_evento_base(interfaceMonitor)) {
            for (const QVariantMap& e : eventos_para_processar) {
                evento_base->registrar_evento_no_banco(e);
            }
            return;
        }

        auto* processador = interfaceMonitor->property("processador_evento").value<source::services::EventoMovido*>();
        if (!processador) return;

        if (eventos_para_processar.size() > 1) {
            QTimer::singleShot(0, processador, [processador, eventos_para_processar]() {
                for (const QVariantMap& e : eventos_para_processar) {
                    processador->adicionar_evento_para_lote(e);
                }
            });
        } else if (!eventos_para_processar.isEmpty()) {
            const QVariantMap evento = eventos_para_processar.first();
            QTimer::singleShot(0, processador, [processador, evento]() {
                processador->processar_evento(evento);
            });
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao processar exclusões pendentes", true);
    }
}

}  // namespace source::services::GerenciamentoEventoMovido
