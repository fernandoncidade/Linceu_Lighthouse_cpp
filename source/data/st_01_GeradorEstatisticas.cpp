#include "st_01_GeradorEstatisticas.h"

#include <QDir>
#include <QDateTime>
#include <QFileInfo>
#include <QHash>
#include <QImage>
#include <QMap>
#include <QMetaObject>
#include <QPair>
#include <QPainter>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>
#include <QVariantList>
#include <QVariantMap>
#include <QVector>

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "source/data/Charts/ch_02_ChartWidgetFactory.h"
#include "source/data/GeradorEstatisticas/gst_01_BaseGerador.h"
#include "source/data/GeradorEstatisticas/gst_02_GraficoPizza.h"
#include "source/data/GeradorEstatisticas/gst_03_GraficoBarras.h"
#include "source/data/GeradorEstatisticas/gst_04_GraficoTimeline.h"
#include "source/data/GeradorEstatisticas/gst_05_GraficoTreemap.h"
#include "source/data/GeradorEstatisticas/gst_06_GraficoHistograma.h"
#include "source/data/GeradorEstatisticas/gst_07_GraficoPareto.h"
#include "source/data/GeradorEstatisticas/gst_08_GraficoLinha.h"
#include "source/data/GeradorEstatisticas/gst_09_GraficoBoxplot.h"
#include "source/data/GeradorEstatisticas/gst_10_GraficoRadarEventos.h"
#include "source/data/GeradorEstatisticas/gst_11_GraficoHeatmap.h"
#include "source/data/GeradorEstatisticas/gst_12_GraficoScatter.h"
#include "source/data/GeradorEstatisticas/gst_13_GraficoSankey.h"
#include "source/data/GeradorEstatisticas/gst_14_GraficoRadar.h"
#include "source/data/GeradorEstatisticas/gst_15_GraficoDotplot.h"
#include "source/data/GeradorEstatisticas/gst_16_GraficoSankeyEventoCaminho.h"
#include "source/data/GeradorEstatisticas/gst_17_GraficoSankeyTipoCaminho.h"
#include "source/data/GeradorEstatisticas/gst_18_GraficoArvoreDiretorios.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
constexpr int kMaxBoxplotOutliersPerBox = 512;
constexpr int kBoxplotFullSortThreshold = 16384;

QString resumir_valor(const QVariant& valor) {
    const int type_id =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        valor.typeId();
#else
        valor.type();
#endif
    switch (type_id) {
        case QMetaType::QVariantList:
            return QStringLiteral("%1 itens").arg(valor.toList().size());
        case QMetaType::QVariantMap:
            return QStringLiteral("%1 chaves").arg(valor.toMap().size());
        case QMetaType::QVariantHash:
            return QStringLiteral("%1 chaves").arg(valor.toHash().size());
        case QMetaType::QString:
            return valor.toString();
        case QMetaType::Bool:
            return valor.toBool() ? QStringLiteral("true") : QStringLiteral("false");
        default:
            if (valor.canConvert<double>()) return QString::number(valor.toDouble());
            return valor.toString();
    }
}

QString criar_texto_preview_grafico(const QString& nome, const QVariant& grafico) {
    QStringList linhas;
    linhas << nome;

    const QVariantMap fig_map = grafico.toMap();
    if (fig_map.isEmpty()) {
        linhas << QStringLiteral("Sem dados de gráfico disponíveis.");
        return linhas.join('\n');
    }

    if (!fig_map.value(QStringLiteral("title")).toString().trimmed().isEmpty()) {
        linhas << fig_map.value(QStringLiteral("title")).toString();
    }
    if (!fig_map.value(QStringLiteral("kind")).toString().trimmed().isEmpty()) {
        linhas << QStringLiteral("Tipo: %1").arg(fig_map.value(QStringLiteral("kind")).toString());
    }
    if (!fig_map.value(QStringLiteral("message")).toString().trimmed().isEmpty()) {
        linhas << fig_map.value(QStringLiteral("message")).toString();
    }

    int detalhes = 0;
    for (auto it = fig_map.constBegin(); it != fig_map.constEnd(); ++it) {
        const QString key = it.key();
        if (key == QStringLiteral("title") || key == QStringLiteral("kind") || key == QStringLiteral("message")) {
            continue;
        }
        linhas << QStringLiteral("%1: %2").arg(key, resumir_valor(it.value()));
        detalhes += 1;
        if (detalhes >= 10) break;
    }

    return linhas.join('\n');
}

bool salvar_preview_png(const QString& arquivo_destino, const QString& texto) {
    QImage image(1280, 720, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::black);
    painter.drawText(image.rect().adjusted(28, 28, -28, -28), Qt::AlignLeft | Qt::TextWordWrap, texto);
    painter.end();

    return image.save(arquivo_destino, "PNG");
}

QVariantMap criar_figura_boxplot_simples(const QString& title,
                                         const QVariantMap& source_map,
                                         const QVariantList& order_hint,
                                         const QVariantMap& color_map,
                                         bool y_log_scale,
                                         const QString& x_label,
                                         const QString& y_label,
                                         int max_items) {
    QVariantMap figure;
    figure.insert(QStringLiteral("kind"), QStringLiteral("boxplot_single"));
    figure.insert(QStringLiteral("title"), title);
    figure.insert(QStringLiteral("source_map"), source_map);
    figure.insert(QStringLiteral("order_hint"), order_hint);
    figure.insert(QStringLiteral("color_map"), color_map);
    figure.insert(QStringLiteral("y_log_scale"), y_log_scale);
    if (!x_label.trimmed().isEmpty()) {
        figure.insert(QStringLiteral("x_label"), x_label);
    }
    if (!y_label.trimmed().isEmpty()) {
        figure.insert(QStringLiteral("y_label"), y_label);
    }
    if (max_items > 0) {
        figure.insert(QStringLiteral("max_items"), max_items);
    }
    return figure;
}

QVariantMap criar_figura_boxplot_precomputado(const QString& title,
                                              const QVariantMap& box_stats,
                                              const QVariantList& order_hint,
                                              const QVariantMap& color_map,
                                              bool y_log_scale,
                                              const QString& x_label,
                                              const QString& y_label,
                                              int max_items) {
    QVariantMap figure;
    figure.insert(QStringLiteral("kind"), QStringLiteral("boxplot_single"));
    figure.insert(QStringLiteral("title"), title);
    figure.insert(QStringLiteral("box_stats"), box_stats);
    figure.insert(QStringLiteral("order_hint"), order_hint);
    figure.insert(QStringLiteral("color_map"), color_map);
    figure.insert(QStringLiteral("y_log_scale"), y_log_scale);
    if (!x_label.trimmed().isEmpty()) {
        figure.insert(QStringLiteral("x_label"), x_label);
    }
    if (!y_label.trimmed().isEmpty()) {
        figure.insert(QStringLiteral("y_label"), y_label);
    }
    if (max_items > 0) {
        figure.insert(QStringLiteral("max_items"), max_items);
    }
    return figure;
}

QString assinatura_arquivos_boxplot(const QString& db_path) {
    const QFileInfo dbInfo(db_path);
    const QFileInfo walInfo(db_path + QStringLiteral("-wal"));
    return QStringLiteral("%1:%2:%3:%4")
        .arg(dbInfo.exists() ? dbInfo.size() : -1)
        .arg(dbInfo.exists() ? dbInfo.lastModified().toMSecsSinceEpoch() : -1)
        .arg(walInfo.exists() ? walInfo.size() : -1)
        .arg(walInfo.exists() ? walInfo.lastModified().toMSecsSinceEpoch() : -1);
}

Qt::ConnectionType invoke_connection_type(QObject* object) {
    if (object == nullptr || object->thread() == QThread::currentThread()) {
        return Qt::DirectConnection;
    }
    return Qt::BlockingQueuedConnection;
}

double variante_para_double(const QVariant& value) {
    bool ok = false;
    double parsed = value.toDouble(&ok);
    if (ok) {
        return parsed;
    }

    QString text = value.toString().trimmed();
    if (text.isEmpty()) {
        return 0.0;
    }
    text.replace(QLatin1Char(','), QLatin1Char('.'));
    parsed = text.toDouble(&ok);
    return ok ? parsed : 0.0;
}

double extrair_bytes_query(const QSqlQuery& query,
                           int idx_size_b,
                           int idx_size_kb,
                           int idx_size_mb,
                           int idx_size_gb,
                           int idx_size_tb) {
    const double sizeB = variante_para_double(query.value(idx_size_b));
    if (sizeB > 0.0) return sizeB;
    const double sizeKb = variante_para_double(query.value(idx_size_kb));
    if (sizeKb > 0.0) return sizeKb * 1024.0;
    const double sizeMb = variante_para_double(query.value(idx_size_mb));
    if (sizeMb > 0.0) return sizeMb * 1024.0 * 1024.0;
    const double sizeGb = variante_para_double(query.value(idx_size_gb));
    if (sizeGb > 0.0) return sizeGb * 1024.0 * 1024.0 * 1024.0;
    const double sizeTb = variante_para_double(query.value(idx_size_tb));
    if (sizeTb > 0.0) return sizeTb * 1024.0 * 1024.0 * 1024.0 * 1024.0;
    return 0.0;
}

QString categoria_tamanho(double bytes) {
    if (bytes < 1024.0) return QStringLiteral("very_small");
    if (bytes < 1024.0 * 1024.0) return QStringLiteral("small");
    if (bytes < 100.0 * 1024.0 * 1024.0) return QStringLiteral("medium");
    if (bytes < 1024.0 * 1024.0 * 1024.0) return QStringLiteral("large");
    return QStringLiteral("very_large");
}

QDateTime extrair_timestamp_estatistica(const QVariant& value) {
    if (value.canConvert<QDateTime>()) {
        const QDateTime dt = value.toDateTime();
        if (dt.isValid()) {
            return dt;
        }
    }

    const QString text = value.toString().trimmed();
    if (text.isEmpty()) {
        return {};
    }

    bool epochOk = false;
    const qlonglong epochRaw = text.toLongLong(&epochOk);
    if (epochOk) {
        const int digits = text.startsWith(QLatin1Char('-')) ? text.size() - 1 : text.size();
        if (digits >= 16) return QDateTime::fromMSecsSinceEpoch(epochRaw / 1000);
        if (digits >= 13) return QDateTime::fromMSecsSinceEpoch(epochRaw);
        return QDateTime::fromSecsSinceEpoch(epochRaw);
    }

    QDateTime dt = QDateTime::fromString(text, Qt::ISODateWithMs);
    if (dt.isValid()) return dt;
    dt = QDateTime::fromString(text, Qt::ISODate);
    if (dt.isValid()) return dt;

    const QString isoGuess = (text.contains(QLatin1Char(' ')) && !text.contains(QLatin1Char('T')))
                                 ? QString(text).replace(QLatin1Char(' '), QLatin1Char('T'))
                                 : text;
    if (isoGuess != text) {
        dt = QDateTime::fromString(isoGuess, Qt::ISODateWithMs);
        if (dt.isValid()) return dt;
        dt = QDateTime::fromString(isoGuess, Qt::ISODate);
        if (dt.isValid()) return dt;
    }

    const QStringList formats = {
        QStringLiteral("yyyy-MM-dd HH:mm:ss"),
        QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"),
        QStringLiteral("yyyy/MM/dd HH:mm:ss"),
        QStringLiteral("yyyy/MM/dd HH:mm:ss.zzz"),
        QStringLiteral("dd/MM/yyyy HH:mm:ss"),
        QStringLiteral("dd/MM/yyyy HH:mm:ss.zzz"),
        QStringLiteral("MM/dd/yyyy HH:mm:ss"),
        QStringLiteral("MM/dd/yyyy HH:mm:ss.zzz"),
        QStringLiteral("dd-MM-yyyy HH:mm:ss"),
        QStringLiteral("dd-MM-yyyy HH:mm:ss.zzz"),
        QStringLiteral("yyyy-MM-dd"),
        QStringLiteral("dd/MM/yyyy"),
        QStringLiteral("MM/dd/yyyy"),
    };
    for (const QString& format : formats) {
        dt = QDateTime::fromString(text, format);
        if (dt.isValid()) {
            return dt;
        }
    }

    return {};
}

QString normalizar_caminho_estatistica(const QString& dir_val, const QString& nome_val = QString()) {
    QString path = dir_val.trimmed();
    const QString name = nome_val.trimmed();
    if (path.isEmpty() && name.isEmpty()) return {};
    if (!path.isEmpty() && !name.isEmpty()) return QDir::cleanPath(QDir(path).filePath(name));
    if (!name.isEmpty()) return QDir::cleanPath(name);
    return QDir::cleanPath(path);
}

struct BoxplotSizeCandidate {
    QString nome;
    double bytes = -1.0;
    qint64 timestampMs = std::numeric_limits<qint64>::min();
};

struct BoxplotEventoRow {
    QString operacao;
    QString operacaoRaw;
    QString tipo;
    QString tipoRaw;
    QString nome;
    QString dirAtual;
    QString dirAnterior;
    double bytes = -1.0;
    qint64 timestampMs = std::numeric_limits<qint64>::min();
    int hora = -1;
};

double escolher_bytes_candidato_boxplot(const QVector<int>& indexes,
                                        const QVector<BoxplotSizeCandidate>& candidates,
                                        const QString& nome,
                                        qint64 targetTimestampMs) {
    if (indexes.isEmpty()) {
        return -1.0;
    }

    const QString trimmedName = nome.trimmed();
    bool found = false;
    double bestBytes = -1.0;
    qint64 bestDistance = std::numeric_limits<qint64>::max();

    const auto consider = [&](int candidateIndex, bool requireSameName) {
        if (candidateIndex < 0 || candidateIndex >= candidates.size()) {
            return;
        }
        const BoxplotSizeCandidate& candidate = candidates.at(candidateIndex);
        if (candidate.bytes <= 0.0) {
            return;
        }

        const bool sameName = trimmedName.isEmpty() || candidate.nome == trimmedName;
        if (requireSameName && !sameName) {
            return;
        }

        qint64 distance = 0;
        if (targetTimestampMs != std::numeric_limits<qint64>::min() &&
            candidate.timestampMs != std::numeric_limits<qint64>::min()) {
            distance = qAbs(candidate.timestampMs - targetTimestampMs);
        }

        if (!found || distance < bestDistance) {
            found = true;
            bestDistance = distance;
            bestBytes = candidate.bytes;
        }
    };

    for (int candidateIndex : indexes) {
        consider(candidateIndex, true);
    }
    if (found) {
        return bestBytes;
    }

    for (int candidateIndex : indexes) {
        consider(candidateIndex, false);
    }
    return bestBytes;
}

double escolher_bytes_por_indice_boxplot(const QHash<QString, QVector<int>>& index,
                                         const QString& key,
                                         const QVector<BoxplotSizeCandidate>& candidates,
                                         const QString& nome,
                                         qint64 targetTimestampMs) {
    if (key.isEmpty()) {
        return -1.0;
    }
    const auto it = index.constFind(key);
    if (it == index.constEnd()) {
        return -1.0;
    }
    return escolher_bytes_candidato_boxplot(*it, candidates, nome, targetTimestampMs);
}

double percentil_ordenado(const QVector<double>& sortedValues, double p) {
    if (sortedValues.isEmpty()) {
        return 0.0;
    }
    const double idx = p * static_cast<double>(sortedValues.size() - 1);
    const int lo = static_cast<int>(std::floor(idx));
    const int hi = static_cast<int>(std::ceil(idx));
    if (lo == hi) {
        return sortedValues.at(lo);
    }
    const double mix = idx - lo;
    return (sortedValues.at(lo) * (1.0 - mix)) + (sortedValues.at(hi) * mix);
}

double valor_ordem(QVector<double>& values, int index) {
    if (values.isEmpty()) {
        return 0.0;
    }
    index = qBound(0, index, values.size() - 1);
    auto nth = values.begin() + index;
    std::nth_element(values.begin(), nth, values.end());
    return values.at(index);
}

double percentil_por_ordem(QVector<double>& values, double p) {
    if (values.isEmpty()) {
        return 0.0;
    }
    const double idx = p * static_cast<double>(values.size() - 1);
    const int lo = static_cast<int>(std::floor(idx));
    const int hi = static_cast<int>(std::ceil(idx));
    const double loValue = valor_ordem(values, lo);
    if (lo == hi) {
        return loValue;
    }
    const double hiValue = valor_ordem(values, hi);
    const double mix = idx - lo;
    return (loValue * (1.0 - mix)) + (hiValue * mix);
}

QVariantList amostrar_outliers(const QVector<double>& outliers) {
    QVariantList result;
    if (outliers.isEmpty()) {
        return result;
    }

    const int total = outliers.size();
    const int limite = qMin(total, kMaxBoxplotOutliersPerBox);
    result.reserve(limite);
    if (total <= kMaxBoxplotOutliersPerBox) {
        for (double value : outliers) {
            result.append(value);
        }
        return result;
    }

    for (int i = 0; i < limite; ++i) {
        const int index = static_cast<int>(
            std::round((static_cast<double>(i) * static_cast<double>(total - 1)) /
                       static_cast<double>(limite - 1)));
        result.append(outliers.at(qBound(0, index, total - 1)));
    }
    return result;
}

QVector<double> selecionar_outliers_amostrados(const QVector<double>& values,
                                               double lowerFence,
                                               double upperFence,
                                               int totalOutliers,
                                               double outlierMin,
                                               double outlierMax) {
    QVector<double> sample;
    const int limit = qMin(totalOutliers, kMaxBoxplotOutliersPerBox);
    if (limit <= 0) {
        return sample;
    }

    sample.reserve(limit);
    QVector<int> targets;
    targets.reserve(limit);
    for (int i = 0; i < limit; ++i) {
        targets.append(static_cast<int>(
            std::round((static_cast<double>(i) * static_cast<double>(totalOutliers - 1)) /
                       static_cast<double>(limit - 1))));
    }

    int ordinal = 0;
    int targetIndex = 0;
    for (double value : values) {
        if (value >= lowerFence && value <= upperFence) {
            continue;
        }
        while (targetIndex < targets.size() && ordinal == targets.at(targetIndex)) {
            sample.append(value);
            ++targetIndex;
        }
        ++ordinal;
        if (targetIndex >= targets.size()) {
            break;
        }
    }

    if (!sample.isEmpty()) {
        sample[0] = outlierMin;
        sample[sample.size() - 1] = outlierMax;
    }
    return sample;
}

QVariantMap calcular_box_stats(QVector<double> values) {
    QVariantMap stats;
    if (values.isEmpty()) {
        return stats;
    }

    const bool usarOrdenacaoCompleta = values.size() <= kBoxplotFullSortThreshold;
    if (usarOrdenacaoCompleta) {
        std::sort(values.begin(), values.end());
    }
    const double q1 = usarOrdenacaoCompleta ? percentil_ordenado(values, 0.25) : percentil_por_ordem(values, 0.25);
    const double median = usarOrdenacaoCompleta ? percentil_ordenado(values, 0.50) : percentil_por_ordem(values, 0.50);
    const double q3 = usarOrdenacaoCompleta ? percentil_ordenado(values, 0.75) : percentil_por_ordem(values, 0.75);
    const double iqr = qMax(0.0, q3 - q1);
    const double lowerFence = q1 - (iqr * 1.5);
    const double upperFence = q3 + (iqr * 1.5);

    bool encontrouWhisker = false;
    double whiskerMin = 0.0;
    double whiskerMax = 0.0;
    QVector<double> outliers;
    outliers.reserve(qMin(values.size(), kMaxBoxplotOutliersPerBox));
    int outlierCount = 0;
    bool encontrouOutlier = false;
    double outlierMin = 0.0;
    double outlierMax = 0.0;
    for (double value : values) {
        if (value < lowerFence || value > upperFence) {
            ++outlierCount;
            if (outlierCount <= kMaxBoxplotOutliersPerBox) {
                outliers.append(value);
            }
            if (!encontrouOutlier) {
                encontrouOutlier = true;
                outlierMin = value;
                outlierMax = value;
            } else {
                outlierMin = qMin(outlierMin, value);
                outlierMax = qMax(outlierMax, value);
            }
            continue;
        }
        if (!encontrouWhisker) {
            encontrouWhisker = true;
            whiskerMin = value;
            whiskerMax = value;
        } else {
            whiskerMin = qMin(whiskerMin, value);
            whiskerMax = qMax(whiskerMax, value);
        }
    }

    if (!encontrouWhisker) {
        const auto minmax = std::minmax_element(values.constBegin(), values.constEnd());
        whiskerMin = *minmax.first;
        whiskerMax = *minmax.second;
        outliers.clear();
        outlierCount = 0;
    } else if (outlierCount > kMaxBoxplotOutliersPerBox) {
        outliers = selecionar_outliers_amostrados(values, lowerFence, upperFence, outlierCount, outlierMin, outlierMax);
    }
    std::sort(outliers.begin(), outliers.end());

    stats.insert(QStringLiteral("sample_count"), values.size());
    stats.insert(QStringLiteral("minimum"), whiskerMin);
    stats.insert(QStringLiteral("q1"), q1);
    stats.insert(QStringLiteral("median"), median);
    stats.insert(QStringLiteral("q3"), q3);
    stats.insert(QStringLiteral("maximum"), whiskerMax);
    stats.insert(QStringLiteral("outliers"), amostrar_outliers(outliers));
    stats.insert(QStringLiteral("outlier_count"), outlierCount);
    return stats;
}

QVariantMap converter_grupos_para_stats(const QHash<QString, QVector<double>>& grupos,
                                        const QVariantList& order_hint,
                                        int max_items) {
    QVariantMap stats;
    QStringList ordered;
    for (const QVariant& hint : order_hint) {
        const QString key = hint.toString().trimmed();
        if (!key.isEmpty() && grupos.contains(key) && !ordered.contains(key)) {
            ordered.append(key);
        }
    }

    QStringList extras;
    for (auto it = grupos.constBegin(); it != grupos.constEnd(); ++it) {
        if (!ordered.contains(it.key())) {
            extras.append(it.key());
        }
    }
    std::sort(extras.begin(), extras.end(), [](const QString& a, const QString& b) {
        return a.localeAwareCompare(b) < 0;
    });
    ordered.append(extras);
    if (max_items > 0 && ordered.size() > max_items) {
        ordered = ordered.mid(0, max_items);
    }

    for (const QString& key : ordered) {
        QVariantMap box = calcular_box_stats(grupos.value(key));
        if (!box.isEmpty()) {
            stats.insert(key, box);
        }
    }
    return stats;
}

QString obter_cor_operacao(QObject* interface_obj, const QString& op_key, const QString& fallback) {
    try {
        QObject* gerenciador_cores = interface_obj ? interface_obj->property("gerenciador_cores").value<QObject*>() : nullptr;
        if (gerenciador_cores == nullptr && interface_obj != nullptr) {
            if (QObject* menus = interface_obj->property("gerenciador_menus_ui").value<QObject*>()) {
                gerenciador_cores = menus->property("gerenciador_cores").value<QObject*>();
            }
        }
        if (gerenciador_cores == nullptr) {
            return fallback;
        }

        QString cor;
        QMetaObject::invokeMethod(gerenciador_cores,
                                  "obter_cor_hex",
                                  invoke_connection_type(gerenciador_cores),
                                  Q_RETURN_ARG(QString, cor),
                                  Q_ARG(QString, op_key));
        return cor.trimmed().isEmpty() ? fallback : cor;
    } catch (...) {
        return fallback;
    }
}

source::ui::LocalizadorQt* obter_localizador(QObject* loc) {
    return qobject_cast<source::ui::LocalizadorQt*>(loc);
}

QString traduzir_tipo_monitorado(QObject* loc, const QString& valor) {
    auto* localizador = obter_localizador(loc);
    if (localizador == nullptr || valor.trimmed().isEmpty()) {
        return valor;
    }

    const QString traduzido = localizador->traduzir_metadados(valor.trimmed());
    return traduzido.trimmed().isEmpty() ? valor : traduzido;
}

QString traduzir_operacao_monitorada(QObject* loc, const QString& valor) {
    auto* localizador = obter_localizador(loc);
    if (localizador == nullptr || valor.trimmed().isEmpty()) {
        return valor;
    }

    const QString traduzido = localizador->traduzir_tipo_operacao(valor.trimmed());
    return traduzido.trimmed().isEmpty() ? valor : traduzido;
}

QPair<QVariantList, QVector<double>> extrair_resumo_radar(const QVariantMap& summary) {
    QVariantList labels;
    QVector<double> values;

    const QVariantList labels_raw = summary.value(QStringLiteral("labels")).toList();
    const QVariantList values_raw = summary.value(QStringLiteral("values")).toList();
    const int count = qMin(labels_raw.size(), values_raw.size());
    for (int i = 0; i < count; ++i) {
        const QString label = labels_raw.at(i).toString().trimmed();
        if (label.isEmpty()) {
            continue;
        }
        labels.append(label);
        values.append(values_raw.at(i).toDouble());
    }

    if (!labels.isEmpty()) {
        return {labels, values};
    }

    for (auto it = summary.constBegin(); it != summary.constEnd(); ++it) {
        if (it.key() == QStringLiteral("labels") || it.key() == QStringLiteral("values")) {
            continue;
        }
        const QString label = it.key().trimmed();
        if (label.isEmpty()) {
            continue;
        }
        labels.append(label);
        values.append(it.value().toDouble());
    }

    return {labels, values};
}
}  // namespace

namespace source::data {

GeradorEstatisticas::GeradorEstatisticas(const QString& db_path, QObject* localizador, QObject* interface_principal)
    : db_path_(db_path), loc_(localizador), interface_(interface_principal) {
    inicializar_geradores();
}

void GeradorEstatisticas::inicializar_geradores() {
    try {
        geradores_.insert("pizza", std::make_shared<GraficoPizza>(db_path_, loc_, interface_));
        geradores_.insert("barras", std::make_shared<GraficoBarras>(db_path_, loc_, interface_));
        geradores_.insert("timeline", std::make_shared<GraficoTimeline>(db_path_, loc_, interface_));
        geradores_.insert("treemap", std::make_shared<GraficoTreemap>(db_path_, loc_, interface_));
        geradores_.insert("histograma", std::make_shared<GraficoHistograma>(db_path_, loc_, interface_));
        geradores_.insert("pareto", std::make_shared<GraficoPareto>(db_path_, loc_, interface_));
        geradores_.insert("linha", std::make_shared<GraficoLinha>(db_path_, loc_, interface_));
        geradores_.insert("boxplot", std::make_shared<GraficoBoxplot>(db_path_, loc_, interface_));
        geradores_.insert("boxplot_eventos", std::make_shared<GraficoRadarEventos>(db_path_, loc_, interface_));
        geradores_.insert("heatmap", std::make_shared<GraficoHeatmap>(db_path_, loc_, interface_));
        geradores_.insert("scatter", std::make_shared<GraficoScatter>(db_path_, loc_, interface_));
        geradores_.insert("sankey", std::make_shared<GraficoSankey>(db_path_, loc_, interface_));
        geradores_.insert("radar", std::make_shared<GraficoRadar>(db_path_, loc_, interface_));
        geradores_.insert("dotplot", std::make_shared<GraficoDotplot>(db_path_, loc_, interface_));
        geradores_.insert("sankey_evento_caminho", std::make_shared<GraficoSankeyEventoCaminho>(db_path_, loc_, interface_));
        geradores_.insert("sankey_tipo_caminho", std::make_shared<GraficoSankeyTipoCaminho>(db_path_, loc_, interface_));
        geradores_.insert("arvore_diretorios", std::make_shared<GraficoArvoreDiretorios>(db_path_, loc_, interface_));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar geradores", true);
    }
}

QString GeradorEstatisticas::texto(const QString& key, const QString& fallback) const {
    if (!loc_) return fallback;
    QString out = fallback;
    QMetaObject::invokeMethod(loc_, "get_text", Q_RETURN_ARG(QString, out), Q_ARG(QString, key), Q_ARG(QString, fallback));
    return out;
}

void GeradorEstatisticas::atualizar_textos_traduzidos() {
    boxplot_preagregado_cache_.clear();
    boxplot_preagregado_cache_assinatura_.clear();
    boxplot_preagregado_cache_valido_ = false;
    for (auto& g : geradores_) {
        if (g) g->atualizar_textos_traduzidos();
    }
}

void GeradorEstatisticas::adicionar_gerador(const QString& nome, FactoryGerador factory_gerador) {
    try {
        if (!factory_gerador) {
            const QString msg = QStringLiteral("Factory inválida para gerador '%1'").arg(nome);
            source::utils::LogManager::get_logger().error(msg);
            throw std::invalid_argument(msg.toStdString());
        }

        const std::shared_ptr<BaseGerador> gerador = factory_gerador(db_path_, loc_, interface_);
        if (!gerador) {
            const QString msg = QStringLiteral("Factory retornou gerador nulo para '%1'").arg(nome);
            source::utils::LogManager::get_logger().error(msg);
            throw std::runtime_error(msg.toStdString());
        }

        geradores_[nome] = gerador;
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            QStringLiteral("Erro ao adicionar gerador '%1'").arg(nome), true);
        throw;
    }
}

void GeradorEstatisticas::remover_gerador(const QString& nome) {
    if (geradores_.contains(nome)) geradores_.remove(nome);
    else source::utils::LogManager::get_logger().warning(QStringLiteral("Tentativa de remover gerador inexistente: %1").arg(nome));
}

QStringList GeradorEstatisticas::listar_geradores() const { return geradores_.keys(); }

QVariant GeradorEstatisticas::gerar_por_chave(const QString& chave, const QString& erro_contexto) {
    try {
        if (!geradores_.contains(chave) || !geradores_.value(chave)) {
            const QString msg = QStringLiteral("Gerador '%1' não encontrado").arg(chave);
            source::utils::LogManager::get_logger().error(msg);
            throw std::runtime_error(msg.toStdString());
        }

        return geradores_.value(chave)->gerar();
    } catch (...) {
        source::utils::LogManager::get_logger().error(erro_contexto, true);
        throw;
    }
}

QVariant GeradorEstatisticas::grafico_operacoes_pizza() { return gerar_por_chave("pizza", "Erro ao gerar gráfico de pizza"); }
QVariant GeradorEstatisticas::grafico_tipos_arquivo_barras() { return gerar_por_chave("barras", "Erro ao gerar gráfico de barras"); }
QVariant GeradorEstatisticas::grafico_timeline_operacoes() { return gerar_por_chave("timeline", "Erro ao gerar gráfico timeline"); }
QVariant GeradorEstatisticas::grafico_treemap_tipos() { return gerar_por_chave("treemap", "Erro ao gerar gráfico treemap"); }
QVariant GeradorEstatisticas::grafico_histograma_horarios() { return gerar_por_chave("histograma", "Erro ao gerar histograma"); }
QVariant GeradorEstatisticas::grafico_pareto_operacoes() { return gerar_por_chave("pareto", "Erro ao gerar gráfico de pareto"); }
QVariant GeradorEstatisticas::grafico_cluster_linha() { return gerar_por_chave("linha", "Erro ao gerar gráfico de linha"); }
QVariant GeradorEstatisticas::grafico_boxplot_distribuicao() { return gerar_por_chave("boxplot", "Erro ao gerar boxplot"); }

QVariantMap GeradorEstatisticas::grafico_boxplot_distribuicao_preagregado() {
    const QString assinatura = assinatura_arquivos_boxplot(db_path_);
    if (boxplot_preagregado_cache_valido_ && boxplot_preagregado_cache_assinatura_ == assinatura) {
        return boxplot_preagregado_cache_;
    }

    QVariantMap out;
    try {
        QHash<QString, QVector<double>> por_operacao;
        QHash<QString, QVector<double>> por_operacao_raw;
        QHash<QString, QVector<double>> por_tipo;
        QHash<QString, QVector<double>> por_tipo_raw;
        QHash<QString, QVector<double>> por_hora;
        QHash<QString, QVector<double>> por_hora_raw;
        QHash<QString, QVector<double>> por_categoria;
        QHash<QString, QVector<double>> por_categoria_raw;
        QVector<BoxplotEventoRow> eventos;
        QVector<BoxplotSizeCandidate> size_candidates;
        QHash<QString, QVector<int>> size_candidates_by_path;
        QHash<QString, QVector<int>> size_candidates_by_name;

        const QString op_added = texto(QStringLiteral("op_added"), QStringLiteral("Adicionado"));
        const QString op_scanned = texto(QStringLiteral("op_scanned"), QStringLiteral("Escaneado"));
        const QString op_deleted = texto(QStringLiteral("op_deleted"), QStringLiteral("Excluído"));
        const QString op_modified = texto(QStringLiteral("op_modified"), QStringLiteral("Modificado"));
        const QString op_moved = texto(QStringLiteral("op_moved"), QStringLiteral("Movido"));
        const QString op_renamed = texto(QStringLiteral("op_renamed"), QStringLiteral("Renomeado"));
        const QVariantList operation_order = {op_added, op_scanned, op_deleted, op_modified, op_moved, op_renamed};

        const QString cat_muito_pequeno = texto(QStringLiteral("very_small"), QStringLiteral("Muito Pequeno"));
        const QString cat_pequeno = texto(QStringLiteral("small"), QStringLiteral("Pequeno"));
        const QString cat_medio = texto(QStringLiteral("medium"), QStringLiteral("Médio"));
        const QString cat_grande = texto(QStringLiteral("large"), QStringLiteral("Grande"));
        const QString cat_muito_grande = texto(QStringLiteral("very_large"), QStringLiteral("Muito Grande"));
        const QHash<QString, QString> category_labels = {
            {QStringLiteral("very_small"), cat_muito_pequeno},
            {QStringLiteral("small"), cat_pequeno},
            {QStringLiteral("medium"), cat_medio},
            {QStringLiteral("large"), cat_grande},
            {QStringLiteral("very_large"), cat_muito_grande},
        };
        const QVariantList category_order = {
            cat_muito_pequeno, cat_pequeno, cat_medio, cat_grande, cat_muito_grande
        };

        QVariantList hour_order;
        QVariantList hour_order_raw;
        const QString hour_format = texto(QStringLiteral("hour_format"), QStringLiteral("%1h"));
        for (int hora = 0; hora < 24; ++hora) {
            hour_order.append(hour_format.arg(QString::number(hora)));
            hour_order_raw.append(QString::number(hora));
        }

        const QString conn_name = QStringLiteral("stats_boxplot_fast_%1_%2")
                                      .arg(reinterpret_cast<quintptr>(this))
                                      .arg(QDateTime::currentMSecsSinceEpoch());
        bool ok = true;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name);
            db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY"));
            db.setDatabaseName(db_path_);
            if (!db.open()) {
                source::utils::LogManager::get_logger().error(
                    QStringLiteral("Erro SQL ao abrir base para boxplot rápido: %1").arg(db.lastError().text()),
                    true);
                ok = false;
            } else {
                QSqlQuery pragma(db);
                pragma.exec(QStringLiteral("PRAGMA query_only=ON"));
                pragma.exec(QStringLiteral("PRAGMA temp_store=MEMORY"));
                pragma.exec(QStringLiteral("PRAGMA cache_size=-65536"));

                QSqlQuery query(db);
                query.setForwardOnly(true);
                query.prepare(QStringLiteral(
                    "SELECT tipo_operacao, tipo, timestamp, size_b, size_kb, size_mb, size_gb, size_tb, "
                    "dir_atual, dir_anterior, nome "
                    "FROM monitoramento WHERE timestamp IS NOT NULL"));
                if (!query.exec()) {
                    source::utils::LogManager::get_logger().error(
                        QStringLiteral("Erro SQL ao consultar boxplots rápidos: %1").arg(query.lastError().text()),
                        true);
                    ok = false;
                } else {
                    while (query.next()) {
                        const QDateTime timestamp = extrair_timestamp_estatistica(query.value(2));
                        const qint64 timestampMs = timestamp.isValid()
                                                       ? timestamp.toMSecsSinceEpoch()
                                                       : std::numeric_limits<qint64>::min();
                        const double bytes = extrair_bytes_query(query, 3, 4, 5, 6, 7);
                        const QString nome = query.value(10).toString().trimmed();
                        const QString dirAtual = normalizar_caminho_estatistica(query.value(8).toString());
                        const QString dirAnterior = normalizar_caminho_estatistica(query.value(9).toString());

                        const QString rawOperacao = query.value(0).toString().trimmed();
                        const QString rawTipo = query.value(1).toString().trimmed();
                        eventos.append(BoxplotEventoRow{
                            traduzir_operacao_monitorada(loc_, rawOperacao),
                            rawOperacao,
                            traduzir_tipo_monitorado(loc_, rawTipo),
                            rawTipo,
                            nome,
                            dirAtual,
                            dirAnterior,
                            bytes,
                            timestampMs,
                            timestamp.isValid() ? timestamp.time().hour() : -1});

                        if (bytes > 0.0) {
                            const int candidateIndex = size_candidates.size();
                            size_candidates.append(BoxplotSizeCandidate{nome, bytes, timestampMs});
                            if (!dirAtual.isEmpty()) {
                                size_candidates_by_path[dirAtual].append(candidateIndex);
                            }
                            if (!dirAnterior.isEmpty() && dirAnterior != dirAtual) {
                                size_candidates_by_path[dirAnterior].append(candidateIndex);
                            }
                            if (!nome.isEmpty()) {
                                size_candidates_by_name[nome].append(candidateIndex);
                            }
                        }
                    }
                }
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(conn_name);
        if (!ok) {
            return {};
        }

        constexpr double bytesPorMb = 1024.0 * 1024.0;
        for (const BoxplotEventoRow& evento : eventos) {
            double bytes = evento.bytes;
            if (bytes <= 0.0) {
                bytes = escolher_bytes_por_indice_boxplot(size_candidates_by_path,
                                                          evento.dirAtual,
                                                          size_candidates,
                                                          evento.nome,
                                                          evento.timestampMs);
            }
            if (bytes <= 0.0) {
                bytes = escolher_bytes_por_indice_boxplot(size_candidates_by_path,
                                                          evento.dirAnterior,
                                                          size_candidates,
                                                          evento.nome,
                                                          evento.timestampMs);
            }
            if (bytes <= 0.0) {
                bytes = escolher_bytes_por_indice_boxplot(size_candidates_by_name,
                                                          evento.nome,
                                                          size_candidates,
                                                          evento.nome,
                                                          evento.timestampMs);
            }
            if (bytes <= 0.0) {
                continue;
            }

            const double mb = bytes / bytesPorMb;
            if (!evento.operacao.isEmpty()) {
                por_operacao[evento.operacao].append(mb);
            }
            if (!evento.operacaoRaw.isEmpty()) {
                por_operacao_raw[evento.operacaoRaw].append(mb);
            }
            if (!evento.tipo.isEmpty()) {
                por_tipo[evento.tipo].append(mb);
            }
            if (!evento.tipoRaw.isEmpty()) {
                por_tipo_raw[evento.tipoRaw].append(mb);
            }
            if (evento.hora >= 0 && evento.hora < 24) {
                por_hora[hour_format.arg(QString::number(evento.hora))].append(mb);
                por_hora_raw[QString::number(evento.hora)].append(mb);
            }

            const QString category_key = categoria_tamanho(bytes);
            por_categoria[category_labels.value(category_key, category_key)].append(mb);
            por_categoria_raw[category_key].append(mb);
        }

        QList<QPair<QString, int>> tipos_ordenados;
        tipos_ordenados.reserve(por_tipo.size());
        for (auto it = por_tipo.constBegin(); it != por_tipo.constEnd(); ++it) {
            tipos_ordenados.append({it.key(), it.value().size()});
        }
        std::sort(tipos_ordenados.begin(), tipos_ordenados.end(),
                  [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
                      if (a.second != b.second) return a.second > b.second;
                      return a.first.localeAwareCompare(b.first) < 0;
                  });
        QVariantList top_types;
        QVariantList top_types_raw;
        for (int i = 0; i < qMin(30, tipos_ordenados.size()); ++i) {
            const QString tipoTraduzido = tipos_ordenados.at(i).first;
            top_types.append(tipoTraduzido);
            QString rawEncontrado;
            for (const BoxplotEventoRow& evento : eventos) {
                if (evento.tipo == tipoTraduzido && !evento.tipoRaw.isEmpty()) {
                    rawEncontrado = evento.tipoRaw;
                    break;
                }
            }
            top_types_raw.append(rawEncontrado.isEmpty() ? tipoTraduzido : rawEncontrado);
        }

        QVariantMap op_colors;
        const QStringList op_keys = {
            QStringLiteral("op_added"), QStringLiteral("op_scanned"), QStringLiteral("op_deleted"),
            QStringLiteral("op_modified"), QStringLiteral("op_moved"), QStringLiteral("op_renamed")
        };
        QVariantList operation_order_raw;
        for (const QString& key : op_keys) {
            operation_order_raw.append(key);
        }
        const QStringList op_labels = {op_added, op_scanned, op_deleted, op_modified, op_moved, op_renamed};
        const QStringList fallback_colors = {
            QStringLiteral("#0000ff"), QStringLiteral("#808080"), QStringLiteral("#ff0000"),
            QStringLiteral("#ff6200"), QStringLiteral("#ff00ff"), QStringLiteral("#00ff00")
        };
        for (int i = 0; i < op_labels.size(); ++i) {
            op_colors.insert(op_labels.at(i), obter_cor_operacao(interface_, op_keys.at(i), fallback_colors.at(i)));
        }
        QVariantMap op_colors_raw;
        for (int i = 0; i < op_labels.size(); ++i) {
            op_colors_raw.insert(op_keys.at(i), op_colors.value(op_labels.at(i)));
        }

        out.insert(QStringLiteral("kind"), QStringLiteral("boxplot_preaggregated"));
        out.insert(QStringLiteral("by_operation_stats"), converter_grupos_para_stats(por_operacao, operation_order, -1));
        out.insert(QStringLiteral("by_operation_stats_raw"),
                   converter_grupos_para_stats(por_operacao_raw, operation_order_raw, -1));
        out.insert(QStringLiteral("operation_order"), operation_order);
        out.insert(QStringLiteral("operation_order_raw"), operation_order_raw);
        out.insert(QStringLiteral("op_colors"), op_colors);
        out.insert(QStringLiteral("op_colors_raw"), op_colors_raw);
        out.insert(QStringLiteral("by_type_stats"), converter_grupos_para_stats(por_tipo, top_types, 30));
        out.insert(QStringLiteral("by_type_stats_raw"), converter_grupos_para_stats(por_tipo_raw, top_types_raw, 30));
        out.insert(QStringLiteral("top_types"), top_types);
        out.insert(QStringLiteral("top_types_raw"), top_types_raw);
        out.insert(QStringLiteral("by_hour_of_day_stats"), converter_grupos_para_stats(por_hora, hour_order, 24));
        out.insert(QStringLiteral("by_hour_of_day_stats_raw"),
                   converter_grupos_para_stats(por_hora_raw, hour_order_raw, 24));
        out.insert(QStringLiteral("hour_order"), hour_order);
        out.insert(QStringLiteral("hour_order_raw"), hour_order_raw);
        out.insert(QStringLiteral("by_size_category_stats"), converter_grupos_para_stats(por_categoria, category_order, 5));
        out.insert(QStringLiteral("by_size_category_stats_raw"),
                   converter_grupos_para_stats(por_categoria_raw,
                                               QVariantList{QStringLiteral("very_small"), QStringLiteral("small"),
                                                            QStringLiteral("medium"), QStringLiteral("large"),
                                                            QStringLiteral("very_large")},
                                               5));
        out.insert(QStringLiteral("category_order"), category_order);
        out.insert(QStringLiteral("category_order_raw"),
                   QVariantList{QStringLiteral("very_small"), QStringLiteral("small"), QStringLiteral("medium"),
                                QStringLiteral("large"), QStringLiteral("very_large")});

        boxplot_preagregado_cache_ = out;
        boxplot_preagregado_cache_assinatura_ = assinatura;
        boxplot_preagregado_cache_valido_ = true;
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            QStringLiteral("Erro ao gerar boxplots pré-agregados"), true);
        return {};
    }
}

QVariant GeradorEstatisticas::grafico_boxplot_distribuicao_individual(const QString& title_key,
                                                                      const QString& title_fallback,
                                                                      const QString& source_key,
                                                                      const QString& order_key,
                                                                      const QString& color_key,
                                                                      bool y_log_scale,
                                                                      const QString& x_label_key,
                                                                      const QString& x_label_fallback,
                                                                      const QString& y_label_key,
                                                                      const QString& y_label_fallback,
                                                                      int max_items) {
    const QString title = texto(title_key, title_fallback);
    if (source_key == QStringLiteral("by_operation") ||
        source_key == QStringLiteral("by_type") ||
        source_key == QStringLiteral("by_hour_of_day") ||
        source_key == QStringLiteral("by_size_category")) {
        const QVariantMap grouped = grafico_boxplot_distribuicao_preagregado();
        const QString stats_key = source_key + QStringLiteral("_stats");
        const QVariantMap box_stats = grouped.value(stats_key).toMap();
        if (box_stats.isEmpty()) {
            return QVariantMap{{QStringLiteral("kind"), QStringLiteral("empty")},
                               {QStringLiteral("title"), title},
                               {QStringLiteral("message"),
                                texto(QStringLiteral("no_data_to_plot"), QStringLiteral("Sem dados para exibir"))}};
        }

        const QVariantList order_hint = grouped.value(order_key).toList();
        const QVariantMap color_map = color_key.trimmed().isEmpty() ? QVariantMap() : grouped.value(color_key).toMap();
        QVariantMap figure =
            criar_figura_boxplot_precomputado(title, box_stats, order_hint, color_map, y_log_scale,
                                              texto(x_label_key, x_label_fallback),
                                              texto(y_label_key, y_label_fallback),
                                              max_items);
        const QString stats_key_raw = stats_key + QStringLiteral("_raw");
        const QString order_key_raw = order_key + QStringLiteral("_raw");
        const QString color_key_raw = color_key.trimmed().isEmpty() ? QString() : color_key + QStringLiteral("_raw");
        const QVariantMap box_stats_raw = grouped.value(stats_key_raw).toMap();
        const QVariantList order_hint_raw = grouped.value(order_key_raw).toList();
        const QVariantMap color_map_raw =
            color_key_raw.trimmed().isEmpty() ? QVariantMap() : grouped.value(color_key_raw).toMap();
        if (!box_stats_raw.isEmpty()) {
            figure.insert(QStringLiteral("box_stats_raw"), box_stats_raw);
        }
        if (!order_hint_raw.isEmpty()) {
            figure.insert(QStringLiteral("order_hint_raw"), order_hint_raw);
        }
        if (!color_map_raw.isEmpty()) {
            figure.insert(QStringLiteral("color_map_raw"), color_map_raw);
        }
        if (source_key == QStringLiteral("by_hour_of_day")) {
            figure.insert(QStringLiteral("boxplot_layout"), QStringLiteral("hour_of_day_compact"));
        }
        return figure;
    }

    const QVariantMap grouped = grafico_boxplot_distribuicao().toMap();
    if (grouped.isEmpty()) {
        return QVariantMap{{QStringLiteral("kind"), QStringLiteral("empty")},
                           {QStringLiteral("title"), title},
                           {QStringLiteral("message"),
                            texto(QStringLiteral("no_data_to_plot"), QStringLiteral("Sem dados para exibir"))}};
    }

    if (grouped.value(QStringLiteral("kind")).toString() == QStringLiteral("empty")) {
        QVariantMap empty = grouped;
        empty.insert(QStringLiteral("title"), title);
        return empty;
    }

    QVariantMap source_map;
    QVariantList order_hint;
    QVariantMap color_map;

    if (source_key == QStringLiteral("by_hour_daily_counts")) {
        const QVariantList by_hour_daily_counts = grouped.value(source_key).toList();
        for (const QVariant& hour_value : by_hour_daily_counts) {
            const QVariantMap hour_map = hour_value.toMap();
            const QString raw_hour = hour_map.value(QStringLiteral("hour")).toString().trimmed();
            const QVariantList counts = hour_map.value(QStringLiteral("counts")).toList();
            if (raw_hour.isEmpty() || counts.isEmpty()) {
                continue;
            }
            const QString hour_label = texto(QStringLiteral("hour_format"), QStringLiteral("%1h")).arg(raw_hour);
            source_map.insert(hour_label, counts);
            order_hint.append(hour_label);
        }
    } else {
        source_map = grouped.value(source_key).toMap();
        order_hint = grouped.value(order_key).toList();
        color_map = color_key.trimmed().isEmpty() ? QVariantMap() : grouped.value(color_key).toMap();
    }

    if (source_map.isEmpty()) {
        return QVariantMap{{QStringLiteral("kind"), QStringLiteral("empty")},
                           {QStringLiteral("title"), title},
                           {QStringLiteral("message"),
                            texto(QStringLiteral("no_data_to_plot"), QStringLiteral("Sem dados para exibir"))}};
    }

    QVariantMap figure =
        criar_figura_boxplot_simples(title, source_map, order_hint, color_map, y_log_scale,
                                     texto(x_label_key, x_label_fallback), texto(y_label_key, y_label_fallback),
                                     max_items);
    if (source_key == QStringLiteral("by_hour_daily_counts") || source_key == QStringLiteral("by_hour_of_day")) {
        figure.insert(QStringLiteral("boxplot_layout"), QStringLiteral("hour_of_day_compact"));
    }
    return figure;
}
QVariant GeradorEstatisticas::grafico_boxplot_distribuicao_por_tipo_operacao() {
    return grafico_boxplot_distribuicao_individual(
        QStringLiteral("size_distribution_by_operation_type"),
        QStringLiteral("Distribuição de Tamanhos por Tipo de Operação"),
        QStringLiteral("by_operation"),
        QStringLiteral("operation_order"),
        QStringLiteral("op_colors"),
        true,
        QStringLiteral("operation_type"),
        QStringLiteral("Tipo de Operação"),
        QStringLiteral("size_megabytes_mb"),
        QStringLiteral("Tamanho (Megabytes) (MB)"),
        -1);
}
QVariant GeradorEstatisticas::grafico_boxplot_distribuicao_por_tipo() {
    return grafico_boxplot_distribuicao_individual(
        QStringLiteral("size_distribution_by_type"),
        QStringLiteral("Distribuição de Tamanhos por Tipo"),
        QStringLiteral("by_type"),
        QStringLiteral("top_types"),
        QString(),
        true,
        QStringLiteral("type"),
        QStringLiteral("Tipo"),
        QStringLiteral("size_megabytes_mb"),
        QStringLiteral("Tamanho (Megabytes) (MB)"),
        30);
}
QVariant GeradorEstatisticas::grafico_boxplot_distribuicao_por_hora_dia() {
    return grafico_boxplot_distribuicao_individual(
        QStringLiteral("size_distribution_by_hour_of_day"),
        QStringLiteral("Distribuição de Tamanhos por Hora do Dia"),
        QStringLiteral("by_hour_of_day"),
        QStringLiteral("hour_order"),
        QString(),
        true,
        QStringLiteral("hour_of_day"),
        QStringLiteral("Hora do Dia"),
        QStringLiteral("size_megabytes_mb"),
        QStringLiteral("Tamanho (Megabytes) (MB)"),
        24);
}
QVariant GeradorEstatisticas::grafico_boxplot_distribuicao_por_categoria_tamanho() {
    return grafico_boxplot_distribuicao_individual(
        QStringLiteral("size_distribution_by_size_categories"),
        QStringLiteral("Distribuição de Tamanhos por Categorias de Tamanho"),
        QStringLiteral("by_size_category"),
        QStringLiteral("category_order"),
        QString(),
        true,
        QStringLiteral("category"),
        QStringLiteral("Categoria"),
        QStringLiteral("size_megabytes_mb"),
        QStringLiteral("Tamanho (Megabytes) (MB)"),
        5);
}
QVariant GeradorEstatisticas::grafico_boxplot_eventos() { return gerar_por_chave("boxplot_eventos", "Erro ao gerar boxplot de eventos"); }
QVariant GeradorEstatisticas::grafico_eventos_monitorados_individual(const QString& title_key,
                                                                     const QString& title_fallback,
                                                                     const QString& source_key,
                                                                     const QColor& color) {
    const QString title = texto(title_key, title_fallback);
    const QVariantMap grouped = grafico_boxplot_eventos().toMap();
    if (grouped.isEmpty()) {
        return QVariantMap{{QStringLiteral("kind"), QStringLiteral("empty")},
                           {QStringLiteral("title"), title},
                           {QStringLiteral("message"),
                            texto(QStringLiteral("no_data_to_plot"), QStringLiteral("Sem dados para exibir"))}};
    }

    if (grouped.value(QStringLiteral("kind")).toString() == QStringLiteral("empty")) {
        QVariantMap empty = grouped;
        empty.insert(QStringLiteral("title"), title);
        return empty;
    }

    QVariantList rawLabels;
    QStringList translatedLabels;
    QVector<double> values;

    if (source_key == QStringLiteral("top_types") || source_key == QStringLiteral("top_operations")) {
        const auto summary = extrair_resumo_radar(grouped.value(source_key).toMap());
        rawLabels = summary.first;
        values = summary.second;
        translatedLabels.reserve(rawLabels.size());
        for (const QVariant& label : rawLabels) {
            const QString text = label.toString();
            translatedLabels.append(source_key == QStringLiteral("top_types")
                                        ? traduzir_tipo_monitorado(loc_, text)
                                        : traduzir_operacao_monitorada(loc_, text));
        }
    } else if (source_key == QStringLiteral("by_weekday")) {
        static const char* weekdayKeys[] = {"weekday_monday",   "weekday_tuesday", "weekday_wednesday",
                                            "weekday_thursday", "weekday_friday",  "weekday_saturday",
                                            "weekday_sunday"};
        static const char* weekdayFallbacks[] = {"Segunda", "Terça", "Quarta", "Quinta",
                                                 "Sexta",   "Sábado", "Domingo"};
        const QVariantList groupedValues = grouped.value(source_key).toList();
        translatedLabels.reserve(7);
        values.reserve(7);
        for (int i = 0; i < 7; ++i) {
            rawLabels.append(QString::fromUtf8(weekdayKeys[i]));
            translatedLabels.append(texto(QString::fromUtf8(weekdayKeys[i]), QString::fromUtf8(weekdayFallbacks[i])));
            values.append((i < groupedValues.size()) ? groupedValues.at(i).toDouble() : 0.0);
        }
    } else if (source_key == QStringLiteral("by_hour")) {
        const QVariantList groupedValues = grouped.value(source_key).toList();
        translatedLabels.reserve(24);
        values.reserve(24);
        const QString hourFormat = texto(QStringLiteral("hour_format"), QStringLiteral("%1h"));
        for (int hour = 0; hour < 24; ++hour) {
            rawLabels.append(QString::number(hour));
            translatedLabels.append(hourFormat.arg(hour));
            values.append((hour < groupedValues.size()) ? groupedValues.at(hour).toDouble() : 0.0);
        }
    }

    if (translatedLabels.isEmpty() || values.isEmpty()) {
        return QVariantMap{{QStringLiteral("kind"), QStringLiteral("empty")},
                           {QStringLiteral("title"), title},
                           {QStringLiteral("message"),
                            texto(QStringLiteral("no_data_to_plot"), QStringLiteral("Sem dados para exibir"))}};
    }

    QVariantMap figure =
        source::charts_lgpl::make_radar_single_figure(title, translatedLabels, values, color, title);
    figure.insert(QStringLiteral("smooth_curve"), false);
    figure.insert(QStringLiteral("mask_grid_under_series"), false);
    figure.insert(QStringLiteral("radar_grid_overlay"), false);
    figure.insert(QStringLiteral("radar_points_foreground"), true);
    figure.insert(QStringLiteral("radar_surface_alpha"), 86);
    figure.insert(QStringLiteral("raw_labels"), rawLabels);
    return figure;
}
QVariant GeradorEstatisticas::grafico_eventos_monitorados_por_tipo() {
    return grafico_eventos_monitorados_individual(
        QStringLiteral("events_monitored_by_type"), QStringLiteral("Eventos Monitorados por Tipo"),
        QStringLiteral("top_types"), QColor(QStringLiteral("#2563eb")));
}
QVariant GeradorEstatisticas::grafico_eventos_monitorados_por_tipo_operacao() {
    return grafico_eventos_monitorados_individual(
        QStringLiteral("events_monitored_by_operation_type"),
        QStringLiteral("Eventos Monitorados por Tipo de Operação"), QStringLiteral("top_operations"),
        QColor(QStringLiteral("#dc2626")));
}
QVariant GeradorEstatisticas::grafico_eventos_monitorados_por_dias_semana() {
    return grafico_eventos_monitorados_individual(
        QStringLiteral("events_monitored_by_week_days"), QStringLiteral("Eventos Monitorados por Dias da Semana"),
        QStringLiteral("by_weekday"), QColor(QStringLiteral("#059669")));
}
QVariant GeradorEstatisticas::grafico_eventos_monitorados_por_hora_dia() {
    return grafico_eventos_monitorados_individual(
        QStringLiteral("events_monitored_by_hour_of_day"), QStringLiteral("Eventos Monitorados por Hora do Dia"),
        QStringLiteral("by_hour"), QColor(QStringLiteral("#7c3aed")));
}
QVariant GeradorEstatisticas::grafico_heatmap() { return gerar_por_chave("heatmap", "Erro ao gerar heatmap"); }
QVariant GeradorEstatisticas::grafico_scatter() { return gerar_por_chave("scatter", "Erro ao gerar scatter"); }
QVariant GeradorEstatisticas::grafico_sankey() { return gerar_por_chave("sankey", "Erro ao gerar sankey"); }
QVariant GeradorEstatisticas::grafico_radar() { return gerar_por_chave("radar", "Erro ao gerar radar"); }
QVariant GeradorEstatisticas::grafico_dotplot() { return gerar_por_chave("dotplot", "Erro ao gerar dotplot"); }
QVariant GeradorEstatisticas::grafico_sankey_evento_caminho() { return gerar_por_chave("sankey_evento_caminho", "Erro ao gerar sankey evento→caminho"); }
QVariant GeradorEstatisticas::grafico_sankey_tipo_caminho() { return gerar_por_chave("sankey_tipo_caminho", "Erro ao gerar sankey tipo→caminho"); }
QVariant GeradorEstatisticas::grafico_arvore_diretorios() { return gerar_por_chave("arvore_diretorios", "Erro ao gerar árvore de diretórios"); }

QVariant GeradorEstatisticas::gerar_grafico(const QString& tipo_grafico) {
    if (!geradores_.contains(tipo_grafico) || !geradores_.value(tipo_grafico)) {
        const QString msg = QStringLiteral("Tipo de gráfico '%1' não encontrado").arg(tipo_grafico);
        source::utils::LogManager::get_logger().error(msg);
        throw std::runtime_error(msg.toStdString());
    }
    return gerar_por_chave(tipo_grafico, QStringLiteral("Erro ao gerar gráfico '%1'").arg(tipo_grafico));
}

QHash<QString, bool> GeradorEstatisticas::salvar_graficos(const QString& diretorio) {
    QHash<QString, bool> resultados;
    const QHash<QString, std::function<QVariant()>> graficos = {
        {texto("operations_pie", "operations_pie"), [this]() { return grafico_operacoes_pizza(); }},
        {texto("bar_chart", "bar_chart"), [this]() { return grafico_tipos_arquivo_barras(); }},
        {texto("timeline", "timeline"), [this]() { return grafico_timeline_operacoes(); }},
        {texto("tree_map", "tree_map"), [this]() { return grafico_treemap_tipos(); }},
        {texto("hour_histogram", "hour_histogram"), [this]() { return grafico_histograma_horarios(); }},
        {texto("pareto_analysis", "pareto_analysis"), [this]() { return grafico_pareto_operacoes(); }},
        {texto("operations_by_day", "operations_by_day"), [this]() { return grafico_cluster_linha(); }},
        {texto("size_distribution_by_operation_type", "size_distribution_by_operation_type"),
         [this]() { return grafico_boxplot_distribuicao_por_tipo_operacao(); }},
        {texto("size_distribution_by_type", "size_distribution_by_type"),
         [this]() { return grafico_boxplot_distribuicao_por_tipo(); }},
        {texto("size_distribution_by_hour_of_day", "size_distribution_by_hour_of_day"),
         [this]() { return grafico_boxplot_distribuicao_por_hora_dia(); }},
        {texto("size_distribution_by_size_categories", "size_distribution_by_size_categories"),
         [this]() { return grafico_boxplot_distribuicao_por_categoria_tamanho(); }},
        {texto("events_monitored_by_type", "events_monitored_by_type"),
         [this]() { return grafico_eventos_monitorados_por_tipo(); }},
        {texto("events_monitored_by_operation_type", "events_monitored_by_operation_type"),
         [this]() { return grafico_eventos_monitorados_por_tipo_operacao(); }},
        {texto("events_monitored_by_week_days", "events_monitored_by_week_days"),
         [this]() { return grafico_eventos_monitorados_por_dias_semana(); }},
        {texto("events_monitored_by_hour_of_day", "events_monitored_by_hour_of_day"),
         [this]() { return grafico_eventos_monitorados_por_hora_dia(); }},
        {texto("temporal_distribution", "temporal_distribution"), [this]() { return grafico_heatmap(); }},
        {texto("file_size_analysis", "file_size_analysis"), [this]() { return grafico_scatter(); }},
        {texto("file_operations_flow", "file_operations_flow"), [this]() { return grafico_sankey(); }},
        {texto("event_to_path_flow", "event_to_path_flow"), [this]() { return grafico_sankey_evento_caminho(); }},
        {texto("type_to_path_flow", "type_to_path_flow"), [this]() { return grafico_sankey_tipo_caminho(); }},
        {texto("operations_by_file_type", "operations_by_file_type"), [this]() { return grafico_radar(); }},
        {texto("file_size_distribution", "file_size_distribution"), [this]() { return grafico_dotplot(); }},
        {texto("directory_tree", "directory_tree"), [this]() { return grafico_arvore_diretorios(); }}
    };

    QDir dir(diretorio);
    if (!dir.exists() && !dir.mkpath(QStringLiteral("."))) {
        source::utils::LogManager::get_logger().error(QStringLiteral("Erro ao criar diretório %1").arg(diretorio));
        for (auto it = graficos.constBegin(); it != graficos.constEnd(); ++it) resultados[it.key()] = false;
        return resultados;
    }
    if (!QFileInfo(dir.absolutePath()).isWritable()) {
        source::utils::LogManager::get_logger().error(
            QStringLiteral("Sem permissão de escrita no diretório %1").arg(diretorio));
        for (auto it = graficos.constBegin(); it != graficos.constEnd(); ++it) resultados[it.key()] = false;
        return resultados;
    }

    for (auto it = graficos.constBegin(); it != graficos.constEnd(); ++it) {
        const QString nome = it.key();
        try {
            const QVariant grafico = it.value()();
            const QString texto_preview = criar_texto_preview_grafico(nome, grafico);
            const bool ok = salvar_preview_png(dir.filePath(nome + QStringLiteral(".png")), texto_preview);
            resultados[nome] = ok;
        } catch (...) {
            source::utils::LogManager::get_logger().error(
                QStringLiteral("Erro ao processar gráfico %1").arg(nome), true);
            resultados[nome] = false;
        }
    }

    return resultados;
}

}  // namespace source::data
