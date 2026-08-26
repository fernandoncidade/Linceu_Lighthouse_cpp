#include "gst_17_GraficoSankeyTipoCaminho.h"

#include <algorithm>

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
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

QString consolidar_para_diretorio(const QString& caminho_absoluto, const QString& tipo_item,
                                  const QString& folder_txt) {
    const QString caminho = QDir::cleanPath(caminho_absoluto.trimmed());
    if (caminho.isEmpty()) return {};

    // Itens de pasta já representam diretórios e devem preservar o próprio caminho.
    if (tipo_item == folder_txt) return caminho;

    const QFileInfo info(caminho);
    const QString parent = QDir::cleanPath(info.path());
    if (parent.isEmpty() || parent == QStringLiteral(".")) return caminho;
    return parent;
}

QList<QString> palette_cores() {
    return {
        QStringLiteral("#377eb8"), QStringLiteral("#4daf4a"), QStringLiteral("#e41a1c"),
        QStringLiteral("#984ea3"), QStringLiteral("#ff7f00"), QStringLiteral("#a65628"),
        QStringLiteral("#f781bf"), QStringLiteral("#999999"), QStringLiteral("#ffff33"),
        QStringLiteral("#a6cee3"), QStringLiteral("#1f78b4"), QStringLiteral("#b2df8a"),
        QStringLiteral("#fb9a99"), QStringLiteral("#cab2d6"), QStringLiteral("#fdbf6f"),
        QStringLiteral("#ff9896"), QStringLiteral("#98df8a"), QStringLiteral("#c5b0d5")};
}
}  // namespace

QVariant GraficoSankeyTipoCaminho::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("type_to_path_flow", QStringLiteral("Fluxo: Tipo (extensão)/Pasta → Caminho"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        struct Registro {
            QString tipo_item;
            QString caminho;
            QDateTime timestamp;
        };
        QList<Registro> registros;
        QHash<QString, QDateTime> ultimo_ts_tipo;
        QHash<QString, QDateTime> ultimo_ts_diretorio;

        const QSet<QString> tipos_folder = {
            QStringLiteral("folder"),  QStringLiteral("pasta"),  QStringLiteral("carpeta"),
            QStringLiteral("dossier"), QStringLiteral("cartella"), QStringLiteral("ordner")};
        const QString folder_txt = obter_texto("folder", QStringLiteral("Pasta"));
        const QString no_ext_txt = obter_texto("no_ext", QStringLiteral("SemExt"));

        auto classificar_tipo = [&](const QString& tipo_val, const QString& nome_val) -> QString {
            const QString tipo = tipo_val.trimmed().toLower();
            if (!tipo.isEmpty() && tipos_folder.contains(tipo)) return folder_txt;
            const QString ext = QFileInfo(nome_val).suffix().trimmed().toLower();
            return ext.isEmpty() ? no_ext_txt : ext;
        };

        auto atualizar_timestamp_mais_recente = [](QHash<QString, QDateTime>& mapa, const QString& chave,
                                                   const QDateTime& ts) {
            if (chave.isEmpty()) return;
            if (!mapa.contains(chave)) {
                mapa.insert(chave, ts);
                return;
            }
            const QDateTime atual = mapa.value(chave);
            if (!atual.isValid() || (ts.isValid() && ts > atual)) mapa.insert(chave, ts);
        };

        for (const QVariant& v : dados) {
            const QVariantMap row = v.toMap();
            const QString tipo_raw = row.value("tipo").toString();
            const QString nome = row.value("nome").toString().trimmed();
            const QString nome_ant =
                row.value("nome_anterior").toString().trimmed().isEmpty() ? nome
                                                                            : row.value("nome_anterior").toString();

            const QString caminho_atual = normalizar_caminho(row.value("dir_atual").toString(), nome);
            const QString caminho_anterior =
                normalizar_caminho(row.value("dir_anterior").toString(), nome_ant);
            const QDateTime ts = extrair_timestamp(row);

            if (!caminho_atual.isEmpty()) {
                const QString tipo_item = classificar_tipo(tipo_raw, nome);
                const QString diretorio = consolidar_para_diretorio(caminho_atual, tipo_item, folder_txt);
                if (diretorio.isEmpty()) continue;
                registros.append({tipo_item, diretorio, ts});
                atualizar_timestamp_mais_recente(ultimo_ts_tipo, tipo_item, ts);
                atualizar_timestamp_mais_recente(ultimo_ts_diretorio, diretorio, ts);
            }
            if (!caminho_anterior.isEmpty() && caminho_anterior != caminho_atual) {
                const QString tipo_item = classificar_tipo(tipo_raw, nome_ant);
                const QString diretorio = consolidar_para_diretorio(caminho_anterior, tipo_item, folder_txt);
                if (diretorio.isEmpty()) continue;
                registros.append({tipo_item, diretorio, ts});
                atualizar_timestamp_mais_recente(ultimo_ts_tipo, tipo_item, ts);
                atualizar_timestamp_mais_recente(ultimo_ts_diretorio, diretorio, ts);
            }
        }
        if (registros.isEmpty()) return _criar_grafico_sem_dados(titulo);

        auto selecionar_mais_recentes = [](const QHash<QString, QDateTime>& mapa, int limite_maximo) {
            QList<QPair<QString, QDateTime>> itens;
            itens.reserve(mapa.size());
            for (auto it = mapa.constBegin(); it != mapa.constEnd(); ++it) itens.append({it.key(), it.value()});

            std::sort(itens.begin(), itens.end(), [](const QPair<QString, QDateTime>& a,
                                                     const QPair<QString, QDateTime>& b) {
                if (a.second.isValid() != b.second.isValid()) return a.second.isValid();
                if (a.second.isValid() && b.second.isValid() && a.second != b.second) return a.second > b.second;
                return a.first.localeAwareCompare(b.first) < 0;
            });

            QList<QString> selecionados;
            const int limite = qMin(limite_maximo, itens.size());
            selecionados.reserve(limite);
            for (int i = 0; i < limite; ++i) selecionados.append(itens.at(i).first);
            return selecionados;
        };

        const QList<QString> tipos_ordenados = selecionar_mais_recentes(ultimo_ts_tipo, 10);
        const QList<QString> caminhos_ordenados = selecionar_mais_recentes(ultimo_ts_diretorio, 10);
        if (tipos_ordenados.isEmpty() || caminhos_ordenados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QSet<QString> tipos_selecionados;
        QSet<QString> caminhos_selecionados;
        for (const QString& tipo : tipos_ordenados) tipos_selecionados.insert(tipo);
        for (const QString& caminho : caminhos_ordenados) caminhos_selecionados.insert(caminho);

        QHash<QString, QHash<QString, int>> fluxos;
        QSet<QString> tipos_com_fluxo;
        QSet<QString> caminhos_com_fluxo;
        for (const Registro& r : registros) {
            if (!tipos_selecionados.contains(r.tipo_item) || !caminhos_selecionados.contains(r.caminho)) continue;
            fluxos[r.tipo_item][r.caminho] += 1;
            tipos_com_fluxo.insert(r.tipo_item);
            caminhos_com_fluxo.insert(r.caminho);
        }
        if (fluxos.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QList<QString> tipos;
        QList<QString> caminhos;
        tipos.reserve(tipos_com_fluxo.size());
        caminhos.reserve(caminhos_com_fluxo.size());
        for (const QString& tipo : tipos_ordenados) {
            if (tipos_com_fluxo.contains(tipo)) tipos.append(tipo);
        }
        for (const QString& caminho : caminhos_ordenados) {
            if (caminhos_com_fluxo.contains(caminho)) caminhos.append(caminho);
        }
        if (tipos.isEmpty() || caminhos.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QHash<QString, QString> caminho_curto;
        QHash<QString, int> contador_rotulos;
        for (const QString& path : caminhos) {
            const QString base = encurtar_label(path);
            const int cnt = contador_rotulos.value(base, 0);
            contador_rotulos[base] = cnt + 1;
            caminho_curto[path] = (cnt == 0) ? base : QStringLiteral("%1 (%2)").arg(base).arg(cnt);
        }

        QVariantList links;
        for (const QString& tipo : tipos) {
            const auto mapa_paths = fluxos.value(tipo);
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
                link.insert("source", tipo);
                link.insert("target", caminho_curto.value(path, path));
                link.insert("target_full_path", path);
                link.insert("value", valor);
                links.append(link);
            }
        }

        QVariantList tipos_labels;
        QVariantList caminho_labels;
        QVariantMap node_colors;
        const QList<QString> pal = palette_cores();
        int idx_tipo = 0;
        for (const QString& tipo : tipos) {
            tipos_labels.append(tipo);
            node_colors.insert(tipo, pal.at(idx_tipo % pal.size()));
            idx_tipo += 1;
        }
        int idx_path = 0;
        for (const QString& path : caminhos) {
            const QString label = caminho_curto.value(path, path);
            caminho_labels.append(label);
            node_colors.insert(label, pal.at((idx_path + 5) % pal.size()));
            idx_path += 1;
        }

        QVariantMap out;
        out.insert("kind", "sankey_tipo_caminho");
        out.insert("title", titulo);
        out.insert("types", tipos_labels);
        out.insert("paths", caminho_labels);
        out.insert("nodes", tipos_labels + caminho_labels);
        out.insert("node_colors", node_colors);
        out.insert("links", links);
        out.insert("subtitle", obter_texto("sankey_type_path_subtitle",
                                           QStringLiteral("10 tipos/extensões mais recentes para 10 diretórios mais recentes")));
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar sankey tipo→caminho", true);
        return _criar_grafico_sem_dados(
            obter_texto("type_to_path_flow", QStringLiteral("Fluxo: Tipo (extensão)/Pasta → Caminho")));
    }
}

}  // namespace source::data
