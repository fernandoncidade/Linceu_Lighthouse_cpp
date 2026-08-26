#include "gst_16_GraficoSankeyEventoCaminho.h"

#include <algorithm>

#include <QDateTime>
#include <QHash>
#include <QList>
#include <QPair>
#include <QSet>

#include "source/utils/LogManager.h"

namespace source::data {

namespace {
QString encurtar_label(const QString& path, int max_len = 60) {
    if (path.size() <= max_len) return path;
    QString p = path;
    p.replace('/', '\\');
    const int last_sep = p.lastIndexOf('\\');
    const QString base = last_sep >= 0 ? p.mid(last_sep + 1) : p;
    if (base.size() >= max_len - 3) return QStringLiteral("...") + base.right(max_len - 3);
    const int prefix = max_len - 3 - base.size();
    return p.left(prefix) + QStringLiteral("...") + base;
}

QList<QString> palette_cores() {
    return {
        QStringLiteral("#377eb8"), QStringLiteral("#4daf4a"), QStringLiteral("#e41a1c"),
        QStringLiteral("#984ea3"), QStringLiteral("#ff7f00"), QStringLiteral("#a65628"),
        QStringLiteral("#f781bf"), QStringLiteral("#999999"), QStringLiteral("#ffff33"),
        QStringLiteral("#a6cee3")};
}
}  // namespace

QVariant GraficoSankeyEventoCaminho::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("event_to_path_flow", QStringLiteral("Fluxo: Evento → Caminho"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        struct Registro {
            QString evento;
            QString caminho;
            QDateTime timestamp;
        };
        QList<Registro> registros;
        QSet<QString> eventos_monitorados;

        for (const QVariant& v : dados) {
            const QVariantMap row = v.toMap();
            const QString evento = row.value("tipo_operacao").toString().trimmed();
            if (evento.isEmpty()) continue;
            eventos_monitorados.insert(evento);

            const QDateTime ts = extrair_timestamp(row);

            const QString caminho_atual = normalizar_caminho(row.value("dir_atual").toString(), row.value("nome").toString());
            const QString nome_base = row.value("nome").toString();
            const QString nome_ant =
                row.value("nome_anterior").toString().trimmed().isEmpty() ? nome_base
                                                                          : row.value("nome_anterior").toString();
            const QString caminho_anterior =
                normalizar_caminho(row.value("dir_anterior").toString(), nome_ant);

            if (!caminho_atual.isEmpty()) registros.append({evento, caminho_atual, ts});
            if (!caminho_anterior.isEmpty() && caminho_anterior != caminho_atual) {
                registros.append({evento, caminho_anterior, ts});
            }
        }
        if (registros.isEmpty()) return _criar_grafico_sem_dados(titulo);

        // Seleciona os últimos caminhos por evento (timestamp mais recente por caminho).
        QHash<QString, QHash<QString, QDateTime>> ultimo_path_por_evento;
        QSet<QString> caminhos_set;
        for (const Registro& r : registros) {
            QDateTime& atual = ultimo_path_por_evento[r.evento][r.caminho];
            if (!atual.isValid() && r.timestamp.isValid()) {
                atual = r.timestamp;
                continue;
            }
            if (!atual.isValid()) continue;
            if (r.timestamp.isValid() && r.timestamp > atual) atual = r.timestamp;
        }

        QHash<QString, QSet<QString>> caminhos_selecionados_por_evento;
        for (const QString& evento : eventos_monitorados) {
            QList<QPair<QString, QDateTime>> candidatos;
            const auto mapa_evento = ultimo_path_por_evento.value(evento);
            candidatos.reserve(mapa_evento.size());
            for (auto it = mapa_evento.constBegin(); it != mapa_evento.constEnd(); ++it) {
                candidatos.append({it.key(), it.value()});
            }

            std::sort(candidatos.begin(), candidatos.end(),
                      [](const QPair<QString, QDateTime>& a, const QPair<QString, QDateTime>& b) {
                          if (a.second.isValid() != b.second.isValid()) return a.second.isValid();
                          if (a.second.isValid() && b.second.isValid() && a.second != b.second) {
                              return a.second > b.second;
                          }
                          return a.first.localeAwareCompare(b.first) < 0;
                      });

            const int limite = qMin(20, candidatos.size());
            for (int i = 0; i < limite; ++i) {
                caminhos_selecionados_por_evento[evento].insert(candidatos.at(i).first);
                caminhos_set.insert(candidatos.at(i).first);
            }
        }

        QHash<QString, QHash<QString, int>> fluxos;
        QSet<QString> eventos_com_fluxo;
        for (const Registro& r : registros) {
            if (!caminhos_selecionados_por_evento.value(r.evento).contains(r.caminho)) continue;
            fluxos[r.evento][r.caminho] += 1;
            eventos_com_fluxo.insert(r.evento);
        }
        if (fluxos.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QList<QString> eventos = eventos_com_fluxo.values();
        QList<QString> caminhos = caminhos_set.values();
        std::sort(eventos.begin(), eventos.end());
        std::sort(caminhos.begin(), caminhos.end());

        QHash<QString, QString> caminho_curto;
        QHash<QString, int> contador_rotulos;
        for (const QString& path : caminhos) {
            const QString base = encurtar_label(path);
            const int cnt = contador_rotulos.value(base, 0);
            contador_rotulos[base] = cnt + 1;
            caminho_curto[path] = (cnt == 0) ? base : QStringLiteral("%1 (%2)").arg(base).arg(cnt);
        }

        QVariantList links;
        for (const QString& evento : eventos) {
            const auto mapa_paths = fluxos.value(evento);
            QList<QString> paths = mapa_paths.keys();
            std::sort(paths.begin(), paths.end(), [&mapa_paths](const QString& a, const QString& b) {
                const int va = mapa_paths.value(a);
                const int vb = mapa_paths.value(b);
                if (va != vb) return va > vb;
                return a.localeAwareCompare(b) < 0;
            });

            for (const QString& path : paths) {
                const int valor = mapa_paths.value(path, 0);
                if (valor <= 0) continue;
                QVariantMap link;
                link.insert("source", evento);
                link.insert("target", caminho_curto.value(path, path));
                link.insert("target_full_path", path);
                link.insert("value", valor);
                links.append(link);
            }
        }

        QVariantList event_labels;
        QVariantList caminho_labels;
        QVariantMap node_colors;
        const QList<QString> pal = palette_cores();
        int idx_pal = 0;
        for (const QString& ev : eventos) {
            event_labels.append(ev);
            QString cor = cores_operacoes_.value(ev);
            if (cor.trimmed().isEmpty()) {
                cor = pal.at(idx_pal % pal.size());
                idx_pal += 1;
            }
            node_colors.insert(ev, cor);
        }

        int idx_caminho = 0;
        for (const QString& path : caminhos) {
            const QString short_label = caminho_curto.value(path, path);
            caminho_labels.append(short_label);
            node_colors.insert(short_label, pal.at(idx_caminho % pal.size()));
            idx_caminho += 1;
        }

        QVariantMap out;
        out.insert("kind", "sankey_evento_caminho");
        out.insert("title", titulo);
        out.insert("events", event_labels);
        out.insert("paths", caminho_labels);
        out.insert("nodes", event_labels + caminho_labels);
        out.insert("node_colors", node_colors);
        out.insert("links", links);
        out.insert("subtitle", obter_texto("sankey_event_path_subtitle",
                                           QStringLiteral("Últimos 20 arquivos/pastas por evento")));
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar sankey evento→caminho", true);
        return _criar_grafico_sem_dados(obter_texto("event_to_path_flow", QStringLiteral("Fluxo: Evento → Caminho")));
    }
}

}  // namespace source::data
