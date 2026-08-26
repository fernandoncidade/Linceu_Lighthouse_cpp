#include "gst_05_GraficoTreemap.h"

#include <algorithm>

#include <QChar>
#include <QDateTime>

#include "source/utils/LogManager.h"

namespace source::data {

namespace {

QString normalizar_operacao(const QString& valor) {
    const QString decomposed = valor.trimmed().normalized(QString::NormalizationForm_D);
    QString cleaned;
    cleaned.reserve(decomposed.size());

    for (const QChar ch : decomposed) {
        const auto category = ch.category();
        if (category == QChar::Mark_NonSpacing || category == QChar::Mark_SpacingCombining ||
            category == QChar::Mark_Enclosing) {
            continue;
        }
        cleaned.append(ch);
    }

    return cleaned.toLower();
}

QString cor_viridis_hex(double t) {
    struct Stop {
        double p;
        int r;
        int g;
        int b;
    };

    const QList<Stop> stops = {
        {0.00, 68, 1, 84},
        {0.25, 59, 82, 139},
        {0.50, 33, 145, 140},
        {0.75, 94, 201, 98},
        {1.00, 253, 231, 37}};

    const double x = qBound(0.0, t, 1.0);
    for (int i = 0; i < stops.size() - 1; ++i) {
        if (x >= stops.at(i).p && x <= stops.at(i + 1).p) {
            const double u = (x - stops.at(i).p) / (stops.at(i + 1).p - stops.at(i).p);
            const int r = static_cast<int>(stops.at(i).r + (stops.at(i + 1).r - stops.at(i).r) * u);
            const int g = static_cast<int>(stops.at(i).g + (stops.at(i + 1).g - stops.at(i).g) * u);
            const int b = static_cast<int>(stops.at(i).b + (stops.at(i + 1).b - stops.at(i).b) * u);
            return QStringLiteral("#%1%2%3")
                .arg(r, 2, 16, QLatin1Char('0'))
                .arg(g, 2, 16, QLatin1Char('0'))
                .arg(b, 2, 16, QLatin1Char('0'));
        }
    }

    return QStringLiteral("#fde725");
}

}  // namespace

QVariant GraficoTreemap::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("tree_map", QStringLiteral("Mapa de Árvore"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        struct Evento {
            QVariantMap row;
            QDateTime ts;
            int ordem;
        };

        QList<Evento> eventos;
        eventos.reserve(dados.size());
        int ordem = 0;
        for (const QVariant& v : dados) {
            const QVariantMap row = v.toMap();
            if (row.isEmpty()) continue;
            eventos.append({row, extrair_timestamp(row), ordem++});
        }

        std::sort(eventos.begin(), eventos.end(), [](const Evento& a, const Evento& b) {
            if (a.ts.isValid() != b.ts.isValid()) return a.ts.isValid();
            if (a.ts.isValid() && b.ts.isValid() && a.ts != b.ts) return a.ts < b.ts;
            return a.ordem < b.ordem;
        });

        const QString op_deleted = normalizar_operacao(obter_texto("op_deleted", QStringLiteral("Excluído")));
        const QString op_renamed = normalizar_operacao(obter_texto("op_renamed", QStringLiteral("Renomeado")));
        const QString op_moved = normalizar_operacao(obter_texto("op_moved", QStringLiteral("Movido")));

        QHash<QString, QString> arquivo_tipo;

        for (const Evento& evento : eventos) {
            const QVariantMap& row = evento.row;
            const QString nome = row.value(QStringLiteral("nome")).toString().trimmed();
            QString tipo = row.value(QStringLiteral("tipo")).toString().trimmed();
            if (tipo.isEmpty()) tipo = obter_texto("unknown", QStringLiteral("Desconhecido"));

            const QString caminho_atual = normalizar_caminho(row.value(QStringLiteral("dir_atual")).toString(), nome);
            const QString caminho_anterior = normalizar_caminho(row.value(QStringLiteral("dir_anterior")).toString(), nome);
            const QString chave_atual =
                !caminho_atual.isEmpty() ? caminho_atual : (!caminho_anterior.isEmpty() ? caminho_anterior : nome);

            const QString op = normalizar_operacao(row.value(QStringLiteral("tipo_operacao")).toString());
            const bool eh_exclusao =
                (op == op_deleted || op == QStringLiteral("deleted") || op == QStringLiteral("excluido"));
            const bool eh_renomeio_ou_mov =
                (op == op_renamed || op == op_moved || op == QStringLiteral("renamed") || op == QStringLiteral("moved"));

            if (eh_exclusao) {
                if (!chave_atual.isEmpty()) arquivo_tipo.remove(chave_atual);
                if (!caminho_anterior.isEmpty()) arquivo_tipo.remove(caminho_anterior);
                continue;
            }

            if (eh_renomeio_ou_mov && !caminho_anterior.isEmpty() && caminho_anterior != chave_atual) {
                arquivo_tipo.remove(caminho_anterior);
            }
            if (!chave_atual.isEmpty()) arquivo_tipo.insert(chave_atual, tipo);
        }

        QHash<QString, int> contagem_tipos;
        for (auto it = arquivo_tipo.constBegin(); it != arquivo_tipo.constEnd(); ++it) {
            const QString tipo = it.value().trimmed();
            if (tipo.isEmpty()) continue;
            contagem_tipos[tipo] += 1;
        }

        // Fallback: se não foi possível reconstruir estado final, usa contagem bruta por evento.
        if (contagem_tipos.isEmpty()) {
            const QVariantMap resumo = criar_resumo_por_coluna(dados, QStringLiteral("tipo"), 30);
            const QVariantList labels_raw = resumo.value(QStringLiteral("labels")).toList();
            const QVariantList values_raw = resumo.value(QStringLiteral("values")).toList();
            for (int i = 0; i < qMin(labels_raw.size(), values_raw.size()); ++i) {
                const QString label = labels_raw.at(i).toString().trimmed();
                const int valor = values_raw.at(i).toInt();
                if (!label.isEmpty() && valor > 0) contagem_tipos[label] += valor;
            }
        }

        QList<QPair<QString, int>> tipos_ordenados;
        tipos_ordenados.reserve(contagem_tipos.size());
        for (auto it = contagem_tipos.constBegin(); it != contagem_tipos.constEnd(); ++it) {
            tipos_ordenados.append({it.key(), it.value()});
        }

        std::sort(tipos_ordenados.begin(), tipos_ordenados.end(),
                  [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
                      if (a.second != b.second) return a.second > b.second;
                      return a.first.toLower() < b.first.toLower();
                  });
        if (tipos_ordenados.size() > 30) tipos_ordenados = tipos_ordenados.mid(0, 30);

        QVariantList labels;
        QVariantList values;
        QVariantList colors;
        labels.reserve(tipos_ordenados.size());
        values.reserve(tipos_ordenados.size());
        colors.reserve(tipos_ordenados.size());

        for (const auto& par : tipos_ordenados) {
            labels.append(par.first);
            values.append(par.second);

            // Cor determinística por rótulo para manter identidade visual estável.
            const uint h = qHash(par.first.toLower());
            const double t = 0.10 + ((h % 900u) / 1000.0);
            colors.append(cor_viridis_hex(t));
        }

        QVariantMap out;
        out.insert("kind", "treemap");
        out.insert("title", titulo);
        out.insert("labels", labels);
        out.insert("values", values);
        out.insert("colors", colors);
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar treemap", true);
        return _criar_grafico_sem_dados(obter_texto("tree_map", QStringLiteral("Mapa de Árvore")));
    }
}

}  // namespace source::data
