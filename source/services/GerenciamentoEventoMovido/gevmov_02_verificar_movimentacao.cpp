#include "gevmov_02_verificar_movimentacao.h"

#include <algorithm>

#include <QDateTime>
#include <QDir>
#include <QMutex>

#include "source/services/GerenciamentoBaseEvento/gbank_00_operacoes.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
QMutex g_mov_mutex;

QList<QVariantMap> to_registros(const QVariantList& lista) {
    QList<QVariantMap> out;
    for (const QVariant& v : lista) out.append(v.toMap());
    return out;
}

QVariantList to_variant_list(const QList<QVariantMap>& lista) {
    QVariantList out;
    for (const QVariantMap& m : lista) out.append(m);
    return out;
}

bool ext_lenta(const QString& nome) {
    static const QStringList exts = {".heic", ".xlsx", ".xls", ".docx", ".pdf", ".mp4", ".mkv", ".mov", ".avi", ".xml", ".exe"};
    const QString n = nome.toLower();
    for (const QString& ext : exts) if (n.endsWith(ext)) return true;
    return false;
}
}

namespace source::services::GerenciamentoEventoMovido {

QString normalizar_caminho(const QString& caminho) {
    return QDir::toNativeSeparators(QDir::cleanPath(caminho)).replace('/', '\\');
}

QVariant verificar_movimentacao(QObject* interfaceMonitor, const QVariant& eventoVar) {
    try {
        QVariantMap evento = eventoVar.toMap();
        if (evento.isEmpty() || !evento.contains("tipo_operacao") || !evento.contains("nome")) {
            return eventoVar;
        }

        if (evento.contains("dir_atual") && !evento.value("dir_atual").toString().isEmpty()) {
            evento["dir_atual"] = normalizar_caminho(evento.value("dir_atual").toString());
        }
        if (evento.contains("dir_anterior") && !evento.value("dir_anterior").toString().isEmpty()) {
            evento["dir_anterior"] = normalizar_caminho(evento.value("dir_anterior").toString());
        }

        if (!interfaceMonitor) return evento;
        if (!interfaceMonitor->property("_aceitar_eventos_monitoramento").toBool() ||
            interfaceMonitor->property("_encerrando_interface").toBool()) {
            return QVariant();
        }
        auto* loc = interfaceMonitor->property("loc").value<source::ui::LocalizadorQt*>();
        if (!loc) return evento;

        QMutex* mutex_ptr = interfaceMonitor->property("mutex").value<QMutex*>();
        QMutexLocker locker(mutex_ptr ? mutex_ptr : &g_mov_mutex);
        QVariantMap excluidos = interfaceMonitor->property("excluidos_recentemente").toMap();

        const QString tipo = source::services::GerenciamentoBaseEvento::chave_canonica_operacao(
            evento.value("tipo_operacao").toString(), loc);
        if (tipo.isEmpty()) {
            return evento;
        }
        evento["tipo_operacao"] = tipo;
        const QString nome = evento.value("nome").toString();

        if (tipo == QStringLiteral("op_deleted")) {
            const double ts = QDateTime::currentMSecsSinceEpoch() / 1000.0;
            QString timestamp_evento = evento.value("timestamp").toString().trimmed();
            if (timestamp_evento.isEmpty()) {
                timestamp_evento = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
                evento["timestamp"] = timestamp_evento;
            }
            // Usa timestamp estável do evento para permitir remoção determinística no DB.
            evento["exclusao_id"] = timestamp_evento;
            evento["_temporario"] = true;

            QList<QVariantMap> lista = to_registros(excluidos.value(nome).toList());
            lista.append({{"timestamp", ts}, {"dir_anterior", evento.value("dir_anterior")}, {"evento", evento}});
            excluidos[nome] = to_variant_list(lista);
            interfaceMonitor->setProperty("excluidos_recentemente", excluidos);
            return QVariant();
        }

        if (tipo == QStringLiteral("op_added") && excluidos.contains(nome)) {
            QList<QVariantMap> lista = to_registros(excluidos.value(nome).toList());
            std::sort(lista.begin(), lista.end(), [](const QVariantMap& a, const QVariantMap& b) {
                return a.value("timestamp").toDouble() > b.value("timestamp").toDouble();
            });

            const double agora = QDateTime::currentMSecsSinceEpoch() / 1000.0;
            bool converteu = false;
            for (int i = 0; i < lista.size(); ++i) {
                const QVariantMap reg = lista.at(i);
                const double excl_time = reg.value("timestamp").toDouble();
                const QString excl_dir = reg.value("dir_anterior").toString();
                const QVariantMap evento_exclusao = reg.value("evento").toMap();

                const double tempo_limite = ext_lenta(nome) ? 5.0 : 0.5;
                if ((agora - excl_time) < tempo_limite) {
                    if (excl_dir != evento.value("dir_atual").toString()) {
                        lista.removeAt(i);
                        if (lista.isEmpty()) excluidos.remove(nome);
                        else excluidos[nome] = to_variant_list(lista);

                        interfaceMonitor->setProperty("excluidos_recentemente", excluidos);

                        evento["tipo_operacao"] = QStringLiteral("op_moved");
                        evento["dir_anterior"] = excl_dir;
                        evento["exclusao_id"] = evento_exclusao.value("exclusao_id");
                        converteu = true;
                    }
                    break;
                }
            }

            if (!converteu) {
                if (lista.isEmpty()) excluidos.remove(nome);
                else excluidos[nome] = to_variant_list(lista);
                interfaceMonitor->setProperty("excluidos_recentemente", excluidos);
            }
        }

        return evento;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro geral em verificar_movimentacao", true);
        return eventoVar;
    }
}

}  // namespace source::services::GerenciamentoEventoMovido
