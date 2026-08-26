#include "gst_18_GraficoArvoreDiretorios.h"

#include <algorithm>

#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QMap>
#include <QSet>

#include "source/utils/LogManager.h"

namespace source::data {

namespace {
QString normalizar_path(const QString& path) {
    QString p = QDir::fromNativeSeparators(path).trimmed();
    if (p.isEmpty() || p == QStringLiteral(".") || p == QStringLiteral("/")) return QString();
    p = QDir::cleanPath(p);
    return p;
}

QString drive_prefix(const QString& path) {
    if (path.size() >= 2 && path.at(1) == QChar(':')) return path.left(2).toUpper();
    if (path.startsWith('/')) return QStringLiteral("/");
    return QString();
}

QString common_path(const QList<QString>& paths) {
    if (paths.isEmpty()) return QString();
    if (paths.size() == 1) return paths.first();

    QList<QStringList> partes;
    partes.reserve(paths.size());
    for (const QString& path : paths) {
        const QString p = QDir::fromNativeSeparators(path);
        QStringList s = p.split('/', Qt::SkipEmptyParts);
        if (p.startsWith('/')) s.prepend(QStringLiteral("/"));
        partes.append(s);
    }

    QStringList prefixo = partes.first();
    for (int i = 1; i < partes.size(); ++i) {
        QStringList novo;
        const QStringList atual = partes.at(i);
        const int n = std::min(prefixo.size(), atual.size());
        for (int j = 0; j < n; ++j) {
            if (prefixo.at(j).compare(atual.at(j), Qt::CaseInsensitive) == 0) {
                novo.append(prefixo.at(j));
            } else {
                break;
            }
        }
        prefixo = novo;
        if (prefixo.isEmpty()) break;
    }

    if (prefixo.isEmpty()) return QString();
    if (prefixo.first() == QStringLiteral("/")) return QStringLiteral("/") + prefixo.mid(1).join('/');
    return prefixo.join('/');
}

int profundidade_relativa(const QString& raiz, const QString& path) {
    if (path == raiz) return 0;
    const QString rel = QDir(raiz).relativeFilePath(path);
    if (rel == QStringLiteral(".") || rel.startsWith("..")) return 0;
    return rel.split('/', Qt::SkipEmptyParts).size();
}

QString encurtar(const QString& txt, int max_len = 20) {
    if (txt.size() <= max_len) return txt;
    return txt.left(max_len - 3) + QStringLiteral("...");
}
}  // namespace

QVariant GraficoArvoreDiretorios::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("directory_tree", QStringLiteral("Árvore de Diretórios"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QSet<QString> dirs_set;
        for (const QVariant& v : dados) {
            const QVariantMap row = v.toMap();
            const QString d1 = normalizar_path(row.value("dir_atual").toString());
            const QString d2 = normalizar_path(row.value("dir_anterior").toString());
            if (!d1.isEmpty()) dirs_set.insert(d1);
            if (!d2.isEmpty()) dirs_set.insert(d2);
        }
        if (dirs_set.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QList<QString> dirs = dirs_set.values();
        std::sort(dirs.begin(), dirs.end());

        QString raiz = common_path(dirs);
        if (raiz.isEmpty()) {
            QHash<QString, int> contagem_drive;
            for (const QString& p : dirs) contagem_drive[drive_prefix(p)] += 1;
            QString drive_escolhido;
            int max_drive = -1;
            for (auto it = contagem_drive.constBegin(); it != contagem_drive.constEnd(); ++it) {
                if (it.value() > max_drive) {
                    max_drive = it.value();
                    drive_escolhido = it.key();
                }
            }
            QList<QString> mesmo_drive;
            for (const QString& p : dirs) {
                if (drive_prefix(p) == drive_escolhido) mesmo_drive.append(p);
            }
            raiz = common_path(mesmo_drive);
            if (raiz.isEmpty()) raiz = drive_escolhido;
        }
        if (raiz.isEmpty()) return _criar_grafico_sem_dados(titulo);
        if (raiz.size() == 2 && raiz.at(1) == QChar(':')) raiz += QStringLiteral("/");

        constexpr int max_depth = 6;
        constexpr int max_nodes = 300;
        QList<QString> nodes;
        nodes.append(raiz);
        QSet<QString> node_set = {raiz};
        QHash<QString, int> link_count;

        auto join_parts = [](const QString& base, const QStringList& parts, int upto) -> QString {
            QString p = base;
            for (int i = 0; i < upto; ++i) p = QDir(p).filePath(parts.at(i));
            return normalizar_path(p);
        };

        for (const QString& d : dirs) {
            if (!normalizar_path(d).startsWith(normalizar_path(raiz), Qt::CaseInsensitive)) continue;

            QString rel = QDir(raiz).relativeFilePath(d);
            rel = QDir::fromNativeSeparators(rel);
            if (rel == QStringLiteral(".") || rel.startsWith("..")) continue;

            QStringList parts = rel.split('/', Qt::SkipEmptyParts);
            if (parts.size() > max_depth) parts = parts.mid(0, max_depth);

            for (int i = 1; i <= parts.size(); ++i) {
                const QString parent = (i == 1) ? raiz : join_parts(raiz, parts, i - 1);
                const QString child = join_parts(raiz, parts, i);
                if (child.isEmpty()) continue;

                if (!node_set.contains(child)) {
                    node_set.insert(child);
                    nodes.append(child);
                    if (nodes.size() >= max_nodes) break;
                }
                link_count[parent + QStringLiteral(" -> ") + child] += 1;
            }
            if (nodes.size() >= max_nodes) break;
        }
        if (nodes.size() <= 1 || link_count.isEmpty()) return _criar_grafico_sem_dados(titulo);

        std::sort(nodes.begin(), nodes.end(), [&](const QString& a, const QString& b) {
            const int da = profundidade_relativa(raiz, a);
            const int db = profundidade_relativa(raiz, b);
            if (da != db) return da < db;
            return a.toLower() < b.toLower();
        });

        QHash<QString, QString> label_por_path;
        QHash<QString, QString> path_por_label;
        QHash<QString, int> contador_label;
        QVariantList node_labels;
        QVariantMap depth_map;

        for (const QString& node : nodes) {
            QString base;
            if (node == raiz) {
                base = QFileInfo(QDir::fromNativeSeparators(node)).fileName();
                if (base.trimmed().isEmpty()) base = node;
            } else {
                base = QFileInfo(QDir::fromNativeSeparators(node)).fileName();
                if (base.trimmed().isEmpty()) base = node;
            }
            base = encurtar(base, 20);
            const int cnt = contador_label.value(base, 0);
            contador_label[base] = cnt + 1;
            const QString label = (cnt == 0) ? base : QStringLiteral("%1 (%2)").arg(base).arg(cnt);

            label_por_path.insert(node, label);
            path_por_label.insert(label, node);
            node_labels.append(label);
            depth_map.insert(label, profundidade_relativa(raiz, node));
        }

        QVariantList links;
        for (auto it = link_count.constBegin(); it != link_count.constEnd(); ++it) {
            const QStringList parts = it.key().split(QStringLiteral(" -> "));
            if (parts.size() != 2) continue;
            const QString source_label = label_por_path.value(parts.at(0));
            const QString target_label = label_por_path.value(parts.at(1));
            if (source_label.isEmpty() || target_label.isEmpty()) continue;

            QVariantMap link;
            link.insert("source", source_label);
            link.insert("target", target_label);
            link.insert("value", it.value());
            links.append(link);
        }
        if (links.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QVariantMap out;
        QVariantMap node_paths;
        for (auto it = path_por_label.constBegin(); it != path_por_label.constEnd(); ++it) {
            node_paths.insert(it.key(), it.value());
        }
        out.insert("kind", "arvore_diretorios");
        out.insert("title", titulo);
        out.insert("root_path", raiz);
        out.insert("nodes", node_labels);
        out.insert("node_paths", node_paths);
        out.insert("depth_map", depth_map);
        out.insert("links", links);
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar árvore de diretórios", true);
        return _criar_grafico_sem_dados(QStringLiteral("Árvore de Diretórios"));
    }
}

}  // namespace source::data
