#include "gevmov_02_verificar_movimentacao.h"

#include <algorithm>

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QMutex>

#include "source/services/GerenciamentoBaseEvento/gbank_00_operacoes.h"
#include "source/services/GerenciamentoEventoMovido/gevmov_10_PendenciasMovimentacao.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
QMutex g_mov_mutex;

bool ext_lenta(const QString& nome) {
    static const QStringList exts = {
        ".heic", ".xlsx", ".xls", ".xlsm", ".docx", ".pdf", ".mp4", ".mkv", ".mov", ".avi",
        ".xml", ".exe", ".lnk", ".db-shm", ".db-wal"
    };
    const QString n = nome.toLower();
    for (const QString& ext : exts) if (n.endsWith(ext)) return true;
    return false;
}

bool ext_aguarda_adicao(const QString& nome) {
    static const QStringList exts = {".lnk", ".xlsm", ".db-shm", ".db-wal"};
    const QString n = nome.toLower();
    for (const QString& ext : exts) if (n.endsWith(ext)) return true;
    return false;
}

bool ext_descarta_exclusao_pos_adicao(const QString& nome) {
    const QString n = nome.toLower();
    return n.endsWith(QStringLiteral(".db-shm")) || n.endsWith(QStringLiteral(".db-wal"));
}

double tempo_limite_correlacao(const QString& nome) {
    return ext_lenta(nome) ? 5.0 : 0.5;
}

bool mesmo_caminho(const QString& lhs, const QString& rhs) {
    const QString left = QDir::toNativeSeparators(QDir::cleanPath(lhs)).replace('/', '\\');
    const QString right = QDir::toNativeSeparators(QDir::cleanPath(rhs)).replace('/', '\\');
    return !left.isEmpty() && left.compare(right, Qt::CaseInsensitive) == 0;
}

QStringList chaves_nome_caminho(const QVariantMap& evento) {
    QStringList chaves;
    const auto adicionar = [&chaves](const QString& valor) {
        const QString normalizado = valor.trimmed();
        if (!normalizado.isEmpty() && !chaves.contains(normalizado, Qt::CaseInsensitive)) {
            chaves.append(normalizado);
        }
    };

    const QString nome = evento.value(QStringLiteral("nome")).toString();
    const QString dir_anterior = evento.value(QStringLiteral("dir_anterior")).toString();
    const QString dir_atual = evento.value(QStringLiteral("dir_atual")).toString();

    adicionar(nome);
    adicionar(QFileInfo(nome).fileName());
    adicionar(dir_anterior);
    adicionar(QFileInfo(dir_anterior).fileName());
    adicionar(dir_atual);
    adicionar(QFileInfo(dir_atual).fileName());
    return chaves;
}

bool remover_pendencias_por_chaves(QHash<QString, QList<QVariantMap>>& pendencias, const QStringList& chaves) {
    bool alterou = false;
    for (const QString& chave : chaves) {
        if (pendencias.remove(chave)) {
            alterou = true;
        }
    }
    return alterou;
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
        auto* pendencias = obter_pendencias_movimentacao(interfaceMonitor);
        if (pendencias == nullptr) {
            return evento;
        }
        auto& excluidos = pendencias->excluidos;
        auto& adicionados = pendencias->adicionados;

        const QString tipo = source::services::GerenciamentoBaseEvento::chave_canonica_operacao(
            evento.value("tipo_operacao").toString(), loc);
        if (tipo.isEmpty()) {
            return evento;
        }
        evento["tipo_operacao"] = tipo;
        const QString nome = evento.value("nome").toString();

        if (tipo == QStringLiteral("op_renamed") || tipo == QStringLiteral("op_moved")) {
            const QStringList chaves = chaves_nome_caminho(evento);
            remover_pendencias_por_chaves(excluidos, chaves);
            remover_pendencias_por_chaves(adicionados, chaves);
            return evento;
        }

        if (tipo == QStringLiteral("op_deleted")) {
            if (adicionados.contains(nome)) {
                QList<QVariantMap> lista = adicionados.value(nome);
                std::sort(lista.begin(), lista.end(), [](const QVariantMap& a, const QVariantMap& b) {
                    return a.value("timestamp").toDouble() > b.value("timestamp").toDouble();
                });

                const double agora = QDateTime::currentMSecsSinceEpoch() / 1000.0;
                for (int i = 0; i < lista.size(); ++i) {
                    const QVariantMap reg = lista.at(i);
                    const double add_time = reg.value("timestamp").toDouble();
                    if ((agora - add_time) >= tempo_limite_correlacao(nome)) {
                        continue;
                    }

                    const QString add_dir = reg.value("dir_atual").toString();
                    const QString del_dir = evento.value("dir_anterior").toString();
                    if (!mesmo_caminho(add_dir, del_dir)) {
                        QVariantMap evento_adicao = reg.value("evento").toMap();
                        lista.removeAt(i);
                        if (lista.isEmpty()) adicionados.remove(nome);
                        else adicionados[nome] = lista;

                        evento_adicao["tipo_operacao"] = QStringLiteral("op_moved");
                        evento_adicao["dir_anterior"] = del_dir;
                        evento_adicao["dir_atual"] = add_dir;
                        if (!evento_adicao.contains("timestamp") ||
                            evento_adicao.value("timestamp").toString().trimmed().isEmpty()) {
                            evento_adicao["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
                        }
                        return evento_adicao;
                    }

                    if (ext_descarta_exclusao_pos_adicao(nome)) {
                        return QVariant();
                    }
                    break;
                }
            }

            const double ts = QDateTime::currentMSecsSinceEpoch() / 1000.0;
            QString timestamp_evento = evento.value("timestamp").toString().trimmed();
            if (timestamp_evento.isEmpty()) {
                timestamp_evento = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
                evento["timestamp"] = timestamp_evento;
            }
            // Usa timestamp estável do evento para permitir remoção determinística no DB.
            evento["exclusao_id"] = timestamp_evento;
            evento["_temporario"] = true;

            QList<QVariantMap> lista = excluidos.value(nome);
            lista.append({{"timestamp", ts}, {"dir_anterior", evento.value("dir_anterior")}, {"evento", evento}});
            excluidos[nome] = lista;
            return QVariant();
        }

        if (tipo == QStringLiteral("op_added") && excluidos.contains(nome)) {
            QList<QVariantMap> lista = excluidos.value(nome);
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

                const double tempo_limite = tempo_limite_correlacao(nome);
                if ((agora - excl_time) < tempo_limite) {
                    if (!mesmo_caminho(excl_dir, evento.value("dir_atual").toString())) {
                        lista.removeAt(i);
                        if (lista.isEmpty()) excluidos.remove(nome);
                        else excluidos[nome] = lista;

                        evento["tipo_operacao"] = QStringLiteral("op_moved");
                        evento["dir_anterior"] = excl_dir;
                        evento["exclusao_id"] = evento_exclusao.value("exclusao_id");
                        converteu = true;
                    } else if (ext_descarta_exclusao_pos_adicao(nome)) {
                        lista.removeAt(i);
                        if (lista.isEmpty()) excluidos.remove(nome);
                        else excluidos[nome] = lista;
                        return evento;
                    }
                    break;
                }
            }

            if (!converteu) {
                if (lista.isEmpty()) excluidos.remove(nome);
                else excluidos[nome] = lista;
            }
        }

        if (tipo == QStringLiteral("op_added") && !excluidos.contains(nome) && ext_aguarda_adicao(nome)) {
            const double ts = QDateTime::currentMSecsSinceEpoch() / 1000.0;
            if (!evento.contains("timestamp") || evento.value("timestamp").toString().trimmed().isEmpty()) {
                evento["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
            }

            QList<QVariantMap> lista = adicionados.value(nome);
            lista.append({{"timestamp", ts}, {"dir_atual", evento.value("dir_atual")}, {"evento", evento}});
            adicionados[nome] = lista;
            return QVariant();
        }

        return evento;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro geral em verificar_movimentacao", true);
        return eventoVar;
    }
}

}  // namespace source::services::GerenciamentoEventoMovido
