#include "gevmov_07_processar_exclusoes_pendentes.h"

#include <memory>

#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <QPointer>
#include <QThreadPool>
#include <QTimer>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/services/ob_08_EventoMovido.h"
#include "source/services/GerenciamentoEventoMovido/gevmov_10_PendenciasMovimentacao.h"
#include "source/utils/LogManager.h"

namespace {
constexpr int kMaxEventosPendentesPorCiclo = 4096;
constexpr int kReagendamentoPendenciasMs = 0;
constexpr double kTempoCorrelacaoNormal = 0.5;
constexpr double kTempoCorrelacaoLenta = 5.0;

bool ext_lenta(const QString& nome) {
    static const QStringList exts = {
        ".heic", ".xlsx", ".xls", ".xlsm", ".docx", ".pdf", ".mp4", ".mkv", ".mov", ".avi",
        ".xml", ".exe", ".lnk", ".db-shm", ".db-wal"
    };
    const QString n = nome.toLower();
    for (const QString& ext : exts) if (n.endsWith(ext)) return true;
    return false;
}

double tempo_limite_correlacao(const QString& nome) {
    return ext_lenta(nome) ? kTempoCorrelacaoLenta : kTempoCorrelacaoNormal;
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

void registrar_eventos_exclusao_em_worker(source::services::BaseEvento* evento_base,
                                          QObject* interfaceMonitor,
                                          QList<QVariantMap> eventos) {
    if (evento_base == nullptr || interfaceMonitor == nullptr || eventos.isEmpty()) {
        return;
    }

    QPointer<QObject> interface_guard(interfaceMonitor);
    QThreadPool::globalInstance()->start([evento_base, interface_guard, eventos = std::move(eventos)]() mutable {
        for (const QVariantMap& evento : eventos) {
            QObject* interface_obj = interface_guard.data();
            if (interface_obj == nullptr ||
                !interface_obj->property("_aceitar_eventos_monitoramento").toBool() ||
                interface_obj->property("_encerrando_interface").toBool()) {
                return;
            }
            evento_base->registrar_evento_no_banco(evento);
        }
    });
}
}

namespace source::services::GerenciamentoEventoMovido {

void processar_exclusoes_pendentes(QObject* interfaceMonitor) {
    try {
        if (!interfaceMonitor) return;

        QMutex* mutex_ptr = interfaceMonitor->property("mutex").value<QMutex*>();
        std::unique_ptr<QMutexLocker<QMutex>> locker;
        if (mutex_ptr != nullptr) {
            locker = std::make_unique<QMutexLocker<QMutex>>(mutex_ptr);
        }

        auto* pendencias = obter_pendencias_movimentacao(interfaceMonitor);
        if (pendencias == nullptr) {
            return;
        }
        auto& excluidos = pendencias->excluidos;
        auto& adicionados = pendencias->adicionados;
        QList<QVariantMap> eventos_para_processar;
        bool ha_pendencias_expiradas = false;
        const double agora = QDateTime::currentMSecsSinceEpoch() / 1000.0;
        const auto tem_orcamento = [&eventos_para_processar]() {
            return eventos_para_processar.size() < kMaxEventosPendentesPorCiclo;
        };

        QStringList nomes_para_remover;
        for (auto it = excluidos.begin(); it != excluidos.end(); ++it) {
            if (!tem_orcamento()) {
                ha_pendencias_expiradas = true;
                break;
            }

            const QString nome = it.key();
            QList<QVariantMap> lista = it.value();
            QList<QVariantMap> remanescentes;

            for (int i = 0; i < lista.size(); ++i) {
                const QVariantMap registro = lista.at(i);
                const double ts = registro.value("timestamp").toDouble();
                const double tempo_limite = tempo_limite_correlacao(nome);

                if ((agora - ts) > tempo_limite) {
                    if (!tem_orcamento()) {
                        remanescentes.append(registro);
                        ha_pendencias_expiradas = true;
                        continue;
                    }
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
            else it.value() = remanescentes;
        }

        for (const QString& n : nomes_para_remover) excluidos.remove(n);

        QStringList adicionados_para_remover;
        for (auto it = adicionados.begin(); it != adicionados.end(); ++it) {
            if (!tem_orcamento()) {
                ha_pendencias_expiradas = true;
                break;
            }

            const QString nome = it.key();
            QList<QVariantMap> lista = it.value();
            QList<QVariantMap> remanescentes;

            for (int i = 0; i < lista.size(); ++i) {
                const QVariantMap registro = lista.at(i);
                const double ts = registro.value("timestamp").toDouble();
                const double tempo_limite = tempo_limite_correlacao(nome);

                if ((agora - ts) > tempo_limite) {
                    if (!tem_orcamento()) {
                        remanescentes.append(registro);
                        ha_pendencias_expiradas = true;
                        continue;
                    }
                    QVariantMap novo_evento = registro.value("evento").toMap();
                    novo_evento.remove("_temporario");
                    if (!novo_evento.contains("timestamp") ||
                        novo_evento.value("timestamp").toString().trimmed().isEmpty()) {
                        novo_evento["timestamp"] =
                            QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(ts * 1000.0))
                                .toString("yyyy-MM-dd HH:mm:ss.zzz");
                    }
                    eventos_para_processar.append(novo_evento);
                } else {
                    remanescentes.append(registro);
                }
            }

            if (remanescentes.isEmpty()) adicionados_para_remover.append(nome);
            else it.value() = remanescentes;
        }

        for (const QString& n : adicionados_para_remover) adicionados.remove(n);

        locker.reset();

        const auto reagendar_se_necessario = [interfaceMonitor, ha_pendencias_expiradas]() {
            if (!ha_pendencias_expiradas || interfaceMonitor == nullptr) {
                return;
            }
            QTimer::singleShot(kReagendamentoPendenciasMs, interfaceMonitor, [interfaceMonitor]() {
                processar_exclusoes_pendentes(interfaceMonitor);
            });
        };

        if (eventos_para_processar.isEmpty()) {
            reagendar_se_necessario();
            return;
        }

        if (auto* evento_base = resolver_evento_base(interfaceMonitor)) {
            registrar_eventos_exclusao_em_worker(evento_base, interfaceMonitor, std::move(eventos_para_processar));
            reagendar_se_necessario();
            return;
        }

        auto* processador = interfaceMonitor->property("processador_evento").value<source::services::EventoMovido*>();
        if (!processador) {
            reagendar_se_necessario();
            return;
        }

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
        reagendar_se_necessario();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao processar exclusões pendentes", true);
    }
}

}  // namespace source::services::GerenciamentoEventoMovido
