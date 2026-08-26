#include "ch_03_LegacyVariantChartAdapter.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <QCoreApplication>
#include <QDate>
#include <QDateTime>
#include <QHash>
#include <QLocale>
#include <QSet>

namespace source::charts_lgpl {
namespace {

double to_double(const QVariant& value) {
    bool ok = false;
    const double result = value.toDouble(&ok);
    return ok ? result : 0.0;
}

QColor color_or(const QVariant& value, const QColor& fallback) {
    const QColor parsed(value.toString());
    return parsed.isValid() ? parsed : fallback;
}

QString fallback_title(const QVariantMap& fig, const QString& title) {
    const QString inside = fig.value(QStringLiteral("title")).toString().trimmed();
    return inside.isEmpty() ? title : inside;
}

QString translate_chart_text(const QString& key, const QString& fallback = QString()) {
    if (key.trimmed().isEmpty()) {
        return fallback;
    }

    const QStringList candidates = {
        key,
        key.toLower(),
        key.toUpper(),
        key.left(1).toUpper() + key.mid(1).toLower()};
    QSet<QString> seen;
    for (const QString& candidate : candidates) {
        if (candidate.isEmpty() || seen.contains(candidate)) {
            continue;
        }
        seen.insert(candidate);
        const QString translated = QCoreApplication::translate("LinceuLighthouse", candidate.toUtf8().constData());
        if (!translated.isEmpty() && (translated != candidate || candidate == key)) {
            return translated;
        }
    }

    return fallback.isNull() ? key : fallback;
}

QString tr_chart(const char* key, const QString& fallback) {
    const QString translated = translate_chart_text(QString::fromUtf8(key), fallback);
    if (translated.trimmed().isEmpty()) {
        return fallback;
    }
    return translated;
}

QString format_chart_number(double value, int fractionalDigits = 2) {
    const double rounded = std::round(value);
    const bool isWhole = std::abs(value - rounded) < 0.0001;
    return QLocale().toString(value, 'f', isWhole ? 0 : fractionalDigits);
}

QString normalize_date_format(const QString& format) {
    QString normalized;
    normalized.reserve(format.size() + 8);
    for (int i = 0; i < format.size();) {
        const QChar ch = format.at(i);
        if (ch == QLatin1Char('\'')) {
            normalized += ch;
            ++i;
            while (i < format.size()) {
                normalized += format.at(i);
                if (format.at(i) == QLatin1Char('\'')) {
                    ++i;
                    break;
                }
                ++i;
            }
            continue;
        }
        if (ch == QLatin1Char('d') || ch == QLatin1Char('M') || ch == QLatin1Char('y')) {
            int j = i + 1;
            while (j < format.size() && format.at(j) == ch) {
                ++j;
            }
            if (ch == QLatin1Char('y')) {
                normalized += QStringLiteral("yyyy");
            } else if (ch == QLatin1Char('d')) {
                normalized += QStringLiteral("dd");
            } else {
                normalized += QStringLiteral("MM");
            }
            i = j;
            continue;
        }
        normalized += ch;
        ++i;
    }
    return normalized;
}

QString normalized_time_format(const QString& format, bool includeSeconds) {
    const bool usesAmPm = format.contains(QStringLiteral("AP")) || format.contains(QStringLiteral("ap"));
    if (usesAmPm) {
        const bool lowerAmPm = format.contains(QStringLiteral("ap"));
        int hourCount = 1;
        const int hourIndex = format.indexOf(QLatin1Char('h'));
        if (hourIndex >= 0) {
            int end = hourIndex + 1;
            while (end < format.size() && format.at(end) == QLatin1Char('h')) {
                ++end;
            }
            hourCount = qMax(1, end - hourIndex);
        }
        return QStringLiteral("%1:mm%2 %3")
            .arg(hourCount >= 2 ? QStringLiteral("hh") : QStringLiteral("h"))
            .arg(includeSeconds ? QStringLiteral(":ss") : QString())
            .arg(lowerAmPm ? QStringLiteral("ap") : QStringLiteral("AP"));
    }

    return QStringLiteral("HH:mm%1").arg(includeSeconds ? QStringLiteral(":ss") : QString());
}

QString localized_datetime_text(const QDateTime& value, bool includeSeconds = true) {
    const QLocale locale;
    const QString dateFormat = normalize_date_format(locale.dateFormat(QLocale::ShortFormat));
    const QString timeFormat = normalized_time_format(locale.timeFormat(QLocale::ShortFormat), includeSeconds);
    return locale.toString(value, QStringLiteral("%1 %2").arg(dateFormat, timeFormat));
}

QString localized_date_text(const QDate& value) {
    const QLocale locale;
    const QString dateFormat = normalize_date_format(locale.dateFormat(QLocale::ShortFormat));
    return locale.toString(value, dateFormat);
}

QString localized_hour_bucket_text(const QVariant& value) {
    bool ok = false;
    const int hour = value.toInt(&ok);
    if (ok) {
        return tr_chart("hour_format", QStringLiteral("%1h")).arg(QLocale().toString(hour));
    }

    const QString text = value.toString().trimmed();
    if (text.isEmpty() || text.endsWith(QLatin1Char('h'), Qt::CaseInsensitive)) {
        return text;
    }
    return tr_chart("hour_format", QStringLiteral("%1h")).arg(text);
}

QString normalized_chart_lookup_text(const QString& text) {
    const QString decomposed = text.normalized(QString::NormalizationForm_D).toLower().simplified();
    QString normalized;
    normalized.reserve(decomposed.size());
    for (const QChar ch : decomposed) {
        const QChar::Category category = ch.category();
        if (category == QChar::Mark_NonSpacing ||
            category == QChar::Mark_SpacingCombining ||
            category == QChar::Mark_Enclosing) {
            continue;
        }
        normalized += ch;
    }
    return normalized;
}

QString translate_size_category_label(const QString& value) {
    if (value.trimmed().isEmpty()) {
        return value;
    }

    const QString normalized = normalized_chart_lookup_text(value);
    struct CategoryAlias {
        const char* key;
        const char* fallback;
        const char* aliases[6];
    };

    static const CategoryAlias categories[] = {
        {"very_small", "Muito Pequeno", {"muito pequeno", "very small", "muy pequeno", "tres petit", "molto piccolo", "sehr klein"}},
        {"small", "Pequeno", {"pequeno", "small", "pequeno", "petit", "piccolo", "klein"}},
        {"medium", "Médio", {"medio", "medium", "mediano", "moyen", "medio", "mittel"}},
        {"large", "Grande", {"grande", "large", "grande", "grand", "grande", "gross"}},
        {"very_large", "Muito Grande", {"muito grande", "very large", "muy grande", "tres grand", "molto grande", "sehr gross"}}};

    for (const auto& category : categories) {
        for (const char* alias : category.aliases) {
            if (normalized == QString::fromUtf8(alias)) {
                return tr_chart(category.key, QString::fromUtf8(category.fallback));
            }
        }
    }

    return value;
}

QVariantMap normalized_chart_value_lists(const QVariantMap& source) {
    QVariantMap normalized;
    for (auto it = source.constBegin(); it != source.constEnd(); ++it) {
        const QString key = it.key().trimmed();
        if (key.isEmpty()) {
            continue;
        }
        QVariantList merged = normalized.value(key).toList();
        merged.append(it.value().toList());
        normalized.insert(key, merged);
    }
    return normalized;
}

QVariantMap normalized_chart_key_map(const QVariantMap& source) {
    QVariantMap normalized;
    for (auto it = source.constBegin(); it != source.constEnd(); ++it) {
        const QString key = it.key().trimmed();
        if (!key.isEmpty()) {
            normalized.insert(key, it.value());
        }
    }
    return normalized;
}

QVariantList normalized_chart_key_list(const QVariantList& source) {
    QVariantList normalized;
    QSet<QString> seen;
    normalized.reserve(source.size());
    for (const QVariant& value : source) {
        const QString key = value.toString().trimmed();
        if (key.isEmpty() || seen.contains(key)) {
            continue;
        }
        seen.insert(key);
        normalized.append(key);
    }
    return normalized;
}

QStringList ordered_keys(const QVariantMap& source, const QVariantList& hints, int maxItems = -1) {
    QStringList ordered;
    ordered.reserve(source.size());
    for (const QVariant& hint : hints) {
        const QString key = hint.toString().trimmed();
        if (!key.isEmpty() && source.contains(key) && !ordered.contains(key)) {
            ordered.append(key);
        }
    }

    QStringList extras;
    for (auto it = source.constBegin(); it != source.constEnd(); ++it) {
        const QString key = it.key().trimmed();
        if (!key.isEmpty() && !ordered.contains(key)) {
            extras.append(key);
        }
    }
    std::sort(extras.begin(), extras.end(), [](const QString& a, const QString& b) {
        return a.localeAwareCompare(b) < 0;
    });
    ordered.append(extras);
    if (maxItems > 0 && ordered.size() > maxItems) {
        ordered = ordered.mid(0, maxItems);
    }
    return ordered;
}

double percentile(const QVector<double>& sortedValues, double p) {
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

ChartDataSet adapt_empty(const QString& title, const QVariantMap& fig) {
    ChartDataSet out;
    out.kind = ChartKind::Empty;
    out.title = fallback_title(fig, title);
    out.subtitle = fig.value(QStringLiteral("message")).toString().trimmed();
    if (out.subtitle.isEmpty()) {
        out.subtitle = QStringLiteral("No data available.");
    }
    return out;
}

ChartDataSet adapt_pie(const QString& title, const QVariantMap& fig) {
    ChartDataSet out;
    out.kind = ChartKind::Pie;
    out.title = fallback_title(fig, title);
    out.meta.insert(QStringLiteral("legendAlignment"), QStringLiteral("right"));
    out.meta.insert(QStringLiteral("legendFromPoints"), true);

    const QVariantList labels = fig.value(QStringLiteral("labels")).toList();
    const QVariantList values = fig.value(QStringLiteral("values")).toList();
    const QVariantList colors = fig.value(QStringLiteral("colors")).toList();

    ChartSeries series;
    series.name = out.title;

    double total = 0.0;
    for (int i = 0; i < qMin(labels.size(), values.size()); ++i) {
        total += qMax(0.0, to_double(values.at(i)));
    }

    const QList<QColor> palette = {
        QColor(QStringLiteral("#0057ff")),
        QColor(QStringLiteral("#ff5400")),
        QColor(QStringLiteral("#00a652")),
        QColor(QStringLiteral("#9d4edd")),
        QColor(QStringLiteral("#d90429")),
        QColor(QStringLiteral("#ffb703"))
    };

    for (int i = 0; i < qMin(labels.size(), values.size()); ++i) {
        const QString label = labels.at(i).toString().trimmed();
        const double value = qMax(0.0, to_double(values.at(i)));
        if (label.isEmpty() || value <= 0.0) {
            continue;
        }

        ChartPoint point;
        point.x = static_cast<double>(series.points.size());
        point.y = value;
        point.label = label;
        point.color = (i < colors.size())
                          ? color_or(colors.at(i), palette.at(i % palette.size()))
                          : palette.at(i % palette.size());
        const double percent = total > 0.0 ? (value / total) * 100.0 : 0.0;
        point.meta.insert(QStringLiteral("percent"), percent);
        point.tooltip = tr_chart("pie_slice_tooltip", QStringLiteral("%1\nQuantity: %2\nParticipation: %3%"))
                            .arg(label)
                            .arg(QString::number(value, 'f', value >= 10.0 ? 0 : 2))
                            .arg(QString::number(percent, 'f', 1));
        series.points.append(point);
    }

    out.series.append(series);
    return out;
}

ChartDataSet adapt_bar_family(const QString& title, const QVariantMap& fig, ChartKind kind) {
    ChartDataSet out;
    out.kind = kind;
    out.title = fallback_title(fig, title);
    out.xLabel = translate_chart_text(fig.value(QStringLiteral("x_label")).toString().trimmed(),
                                      fig.value(QStringLiteral("x_label")).toString().trimmed());
    out.yLabel = translate_chart_text(fig.value(QStringLiteral("y_label")).toString().trimmed(),
                                      fig.value(QStringLiteral("y_label")).toString().trimmed());
    if (kind == ChartKind::Bars && out.xLabel.isEmpty()) {
        out.xLabel = tr_chart("type", QStringLiteral("Tipo"));
    } else if (kind == ChartKind::Histogram && out.xLabel.isEmpty()) {
        out.xLabel = tr_chart("hour_of_day", QStringLiteral("Hora do Dia"));
    }
    if (out.yLabel.isEmpty()) {
        if (kind == ChartKind::Histogram) {
            out.yLabel = tr_chart("event_quantity", QStringLiteral("Quantidade de Eventos"));
        } else {
            out.yLabel = (kind == ChartKind::Bars) ? tr_chart("quantity", QStringLiteral("Quantidade"))
                                                   : tr_chart("value", QStringLiteral("Valor"));
        }
    }

    const QVariantList labels = fig.value(QStringLiteral("labels")).toList();
    const QVariantList values = fig.value(QStringLiteral("values")).toList();
    const QVariantList colors = fig.value(QStringLiteral("colors")).toList();

    ChartSeries series;
    series.name = fig.value(QStringLiteral("series_name")).toString().trimmed();
    if (series.name.isEmpty()) {
        series.name = out.title;
    }
    series.role = QStringLiteral("bar");

    const QList<QColor> palette = {
        QColor(QStringLiteral("#0057ff")),
        QColor(QStringLiteral("#888888")),
        QColor(QStringLiteral("#ff3b30")),
        QColor(QStringLiteral("#ff7a00")),
        QColor(QStringLiteral("#7c3aed")),
        QColor(QStringLiteral("#16a34a"))
    };

    for (int i = 0; i < qMin(labels.size(), values.size()); ++i) {
        const QVariant labelValue = labels.at(i);
        const QString label = (kind == ChartKind::Histogram)
                                  ? localized_hour_bucket_text(labelValue)
                                  : translate_chart_text(labelValue.toString().trimmed(), labelValue.toString().trimmed());
        const double value = qMax(0.0, to_double(values.at(i)));
        if (label.isEmpty()) {
            continue;
        }

        out.categories.append(label);
        ChartPoint point;
        point.x = static_cast<double>(series.points.size());
        point.y = value;
        point.label = label;
        point.color = (i < colors.size())
                          ? color_or(colors.at(i), palette.at(i % palette.size()))
                          : palette.at(i % palette.size());
        const QString numericValue = format_chart_number(value, 2);
        point.tooltip = (kind == ChartKind::Histogram)
                            ? tr_chart("hour_quantity_tooltip", QStringLiteral("Hour: %1\nQuantity: %2"))
                                  .arg(label)
                                  .arg(numericValue)
                            : tr_chart("label_quantity_tooltip", QStringLiteral("%1\nQuantity: %2"))
                                  .arg(label)
                                  .arg(numericValue);
        point.meta.insert(QStringLiteral("legendLabel"), QStringLiteral("%1 - %2").arg(numericValue, label));
        point.meta.insert(QStringLiteral("legendTooltip"), point.tooltip);
        series.points.append(point);
    }

    if (kind == ChartKind::Bars || kind == ChartKind::Histogram) {
        series.meta.insert(QStringLiteral("usePointColors"), true);
        out.meta.insert(QStringLiteral("legendFromPoints"), true);
    }

    out.series.append(series);
    return out;
}

ChartDataSet adapt_pareto(const QString& title, const QVariantMap& fig) {
    ChartDataSet out = adapt_bar_family(title, fig, ChartKind::Pareto);
    out.xLabel = translate_chart_text(fig.value(QStringLiteral("x_label")).toString().trimmed(),
                                      fig.value(QStringLiteral("x_label")).toString().trimmed());
    if (out.xLabel.isEmpty()) {
        out.xLabel = tr_chart("operation_type", QStringLiteral("Tipo de Operação"));
    }
    out.yLabel = translate_chart_text(fig.value(QStringLiteral("y_label")).toString().trimmed(),
                                      fig.value(QStringLiteral("y_label")).toString().trimmed());
    if (out.yLabel.isEmpty()) {
        out.yLabel = tr_chart("quantity", QStringLiteral("Quantidade"));
    }
    out.y2Label = translate_chart_text(fig.value(QStringLiteral("y2_label")).toString().trimmed(),
                                       fig.value(QStringLiteral("y2_label")).toString().trimmed());
    if (out.y2Label.isEmpty()) {
        out.y2Label = tr_chart("cumulative_percentage", QStringLiteral("Porcentagem Acumulada"));
    }
    out.meta.insert(QStringLiteral("legendFromPoints"), true);
    if (!out.series.isEmpty()) {
        out.series[0].meta.insert(QStringLiteral("usePointColors"), true);
    }

    const QLocale locale;
    if (!out.series.isEmpty()) {
        ChartSeries& barSeries = out.series[0];
        double total = 0.0;
        for (const ChartPoint& point : barSeries.points) {
            total += qMax(0.0, point.y);
        }

        QVector<double> cumulativePercents;
        cumulativePercents.reserve(barSeries.points.size());
        double running = 0.0;
        for (int i = 0; i < barSeries.points.size(); ++i) {
            running += qMax(0.0, barSeries.points.at(i).y);
            double cumulativePercent = total > 0.0 ? (running / total) * 100.0 : 0.0;
            if (i == (barSeries.points.size() - 1) && total > 0.0) {
                cumulativePercent = 100.0;
            }
            cumulativePercents.append(cumulativePercent);
        }

        for (int i = 0; i < barSeries.points.size(); ++i) {
            ChartPoint& point = barSeries.points[i];
            const QString quantityText = format_chart_number(point.y, 2);
            const QString cumulativeText = locale.toString(cumulativePercents.at(i), 'f', 2);
            point.tooltip = tr_chart("pareto_bar_tooltip", QStringLiteral("%1\nQuantity: %2\nCumulative: %3%"))
                                .arg(point.label)
                                .arg(quantityText)
                                .arg(cumulativeText);
            point.meta.insert(QStringLiteral("legendTooltip"), point.tooltip);
        }

        ChartSeries cumulativeSeries;
        cumulativeSeries.name = tr_chart("cumulative_percent_short", QStringLiteral("Acumulado (%)"));
        cumulativeSeries.role = QStringLiteral("line_secondary");
        cumulativeSeries.color = color_or(fig.value(QStringLiteral("line_color")), QColor(QStringLiteral("#111827")));

        for (int i = 0; i < qMin(out.categories.size(), cumulativePercents.size()); ++i) {
            ChartPoint point;
            point.x = static_cast<double>(i);
            point.y = cumulativePercents.at(i);
            point.label = out.categories.at(i);
            point.color = cumulativeSeries.color;
            point.tooltip = tr_chart("pareto_line_tooltip", QStringLiteral("Cumulative\nType: %1\nPercentage: %2%"))
                                .arg(point.label)
                                .arg(locale.toString(point.y, 'f', 2));
            cumulativeSeries.points.append(point);
        }
        out.series.append(cumulativeSeries);
    }
    return out;
}

ChartDataSet adapt_combo_bars_line(const QString& title, const QVariantMap& fig) {
    ChartDataSet out;
    out.kind = ChartKind::ComboBarsLine;
    out.title = fallback_title(fig, title);
    out.xLabel = translate_chart_text(fig.value(QStringLiteral("x_label")).toString().trimmed(),
                                      fig.value(QStringLiteral("x_label")).toString().trimmed());
    if (out.xLabel.isEmpty()) {
        out.xLabel = tr_chart("date", QStringLiteral("Data"));
    }
    out.yLabel = translate_chart_text(fig.value(QStringLiteral("y_label")).toString().trimmed(),
                                      fig.value(QStringLiteral("y_label")).toString().trimmed());
    if (out.yLabel.isEmpty()) {
        out.yLabel = tr_chart("quantity_by_type", QStringLiteral("Quantidade por Tipo"));
    }
    out.y2Label = translate_chart_text(fig.value(QStringLiteral("y2_label")).toString().trimmed(),
                                       fig.value(QStringLiteral("y2_label")).toString().trimmed());
    if (out.y2Label.isEmpty()) {
        out.y2Label = tr_chart("total_operations", QStringLiteral("Total de Operações"));
    }
    out.meta.insert(QStringLiteral("legendAlignment"), QStringLiteral("right"));

    const QVariantList rows = fig.value(QStringLiteral("series")).toList();
    const QVariantMap opColors = fig.value(QStringLiteral("op_colors")).toMap();
    const QVariantList opOrder = fig.value(QStringLiteral("operation_order")).toList();

    QVector<QVariantMap> operationsByDay;
    QVector<double> totalsByDay;
    QHash<QString, double> totalsByOperation;
    QSet<QString> foundOps;
    for (const QVariant& rowVar : rows) {
        const QVariantMap row = rowVar.toMap();
        QString dateLabel = row.value(QStringLiteral("date")).toString().trimmed();
        QDate date = QDate::fromString(dateLabel, Qt::ISODate);
        if (!date.isValid()) {
            date = QDate::fromString(dateLabel, QStringLiteral("dd/MM/yyyy"));
        }
        if (date.isValid()) {
            dateLabel = localized_date_text(date);
        }
        if (dateLabel.isEmpty()) {
            continue;
        }

        QVariantMap opsMap = row.value(QStringLiteral("operations")).toMap();
        if (opsMap.isEmpty()) {
            const QVariantHash opsHash = row.value(QStringLiteral("operations")).toHash();
            for (auto it = opsHash.constBegin(); it != opsHash.constEnd(); ++it) {
                opsMap.insert(it.key(), it.value());
            }
        }

        out.categories.append(dateLabel);
        operationsByDay.append(opsMap);
        totalsByDay.append(to_double(row.value(QStringLiteral("total"))));

        for (auto it = opsMap.constBegin(); it != opsMap.constEnd(); ++it) {
            const QString op = it.key().trimmed();
            if (!op.isEmpty()) {
                foundOps.insert(op);
                totalsByOperation[op] += to_double(it.value());
            }
        }
    }

    QStringList orderedOps;
    for (const QVariant& item : opOrder) {
        const QString op = item.toString().trimmed();
        if (!op.isEmpty() && foundOps.contains(op) && !orderedOps.contains(op)) {
            orderedOps.append(op);
        }
    }
    QStringList extras = foundOps.values();
    std::sort(extras.begin(), extras.end(), [](const QString& a, const QString& b) {
        return a.localeAwareCompare(b) < 0;
    });
    for (const QString& op : extras) {
        if (!orderedOps.contains(op)) {
            orderedOps.append(op);
        }
    }

    const QList<QColor> fallbackPalette = {
        QColor(QStringLiteral("#0057ff")),
        QColor(QStringLiteral("#888888")),
        QColor(QStringLiteral("#ff3b30")),
        QColor(QStringLiteral("#ff7a00")),
        QColor(QStringLiteral("#e600ff")),
        QColor(QStringLiteral("#00aa55"))
    };
    const QLocale locale;
    const auto quantityTextFor = [&](double value) {
        return format_chart_number(value, 2);
    };

    QStringList ascendingByTotal = orderedOps;
    std::sort(ascendingByTotal.begin(), ascendingByTotal.end(), [&](const QString& a, const QString& b) {
        const double totalA = totalsByOperation.value(a);
        const double totalB = totalsByOperation.value(b);
        if (std::abs(totalA - totalB) > 0.0001) {
            return totalA < totalB;
        }
        return a.localeAwareCompare(b) < 0;
    });
    double cumulativeTotalByOperation = 0.0;
    for (const QString& op : ascendingByTotal) {
        cumulativeTotalByOperation += qMax(0.0, totalsByOperation.value(op));
    }
    if (cumulativeTotalByOperation > 0.0) {
        out.meta.insert(QStringLiteral("y2Max"), cumulativeTotalByOperation);
    }

    for (int opIndex = 0; opIndex < orderedOps.size(); ++opIndex) {
        const QString op = orderedOps.at(opIndex);
        ChartSeries series;
        const QString displayOp = translate_chart_text(op, op);
        const double totalByOp = totalsByOperation.value(op);
        series.name = displayOp;
        series.role = QStringLiteral("bar");
        series.color = color_or(opColors.value(op), fallbackPalette.at(opIndex % fallbackPalette.size()));
        series.meta.insert(QStringLiteral("legendLabel"), QStringLiteral("%1 - %2").arg(quantityTextFor(totalByOp), displayOp));
        series.meta.insert(QStringLiteral("legendTooltip"),
                           tr_chart("label_quantity_tooltip", QStringLiteral("%1\nQuantity: %2"))
                               .arg(displayOp)
                               .arg(quantityTextFor(totalByOp)));
        for (int dayIndex = 0; dayIndex < operationsByDay.size(); ++dayIndex) {
            ChartPoint point;
            point.x = static_cast<double>(dayIndex);
            point.y = to_double(operationsByDay.at(dayIndex).value(op));
            point.label = out.categories.value(dayIndex);
            point.color = series.color;
            point.tooltip = tr_chart("line_day_tooltip", QStringLiteral("%1\nDate: %2\nQuantity: %3\nDay total: %4"))
                                .arg(displayOp)
                                .arg(point.label)
                                .arg(quantityTextFor(point.y))
                                .arg(quantityTextFor(totalsByDay.value(dayIndex)));
            series.points.append(point);
        }
        out.series.append(series);
    }

    ChartSeries totalSeries;
    totalSeries.name = out.y2Label;
    totalSeries.role = QStringLiteral("line_secondary");
    totalSeries.color = QColor(QStringLiteral("#111827"));
    const double overallTotal = std::accumulate(totalsByDay.begin(), totalsByDay.end(), 0.0);
    totalSeries.meta.insert(QStringLiteral("legendLabel"),
                            QStringLiteral("%1 - %2").arg(quantityTextFor(overallTotal), out.y2Label));
    totalSeries.meta.insert(QStringLiteral("legendTooltip"),
                            tr_chart("label_quantity_tooltip", QStringLiteral("%1\nQuantity: %2"))
                                .arg(out.y2Label)
                                .arg(quantityTextFor(overallTotal)));
    for (int i = 0; i < totalsByDay.size(); ++i) {
        ChartPoint point;
        point.x = static_cast<double>(i);
        point.y = totalsByDay.at(i);
        point.label = out.categories.value(i);
        point.color = totalSeries.color;
        point.tooltip = tr_chart("line_total_tooltip", QStringLiteral("%1\nDate: %2\nDay total: %3"))
                            .arg(totalSeries.name)
                            .arg(point.label)
                            .arg(quantityTextFor(point.y));
        totalSeries.points.append(point);
    }
    out.series.append(totalSeries);
    return out;
}

ChartDataSet adapt_timeline_like(const QString& title, const QVariantMap& fig, ChartKind kind) {
    ChartDataSet out;
    out.kind = kind;
    out.title = fallback_title(fig, title);
    out.xLabel = translate_chart_text(fig.value(QStringLiteral("x_label")).toString().trimmed(),
                                      fig.value(QStringLiteral("x_label")).toString().trimmed());
    out.yLabel = translate_chart_text(fig.value(QStringLiteral("y_label")).toString().trimmed(),
                                      fig.value(QStringLiteral("y_label")).toString().trimmed());
    if (out.xLabel.isEmpty()) {
        out.xLabel = tr_chart("time_label", QStringLiteral("Tempo"));
    }
    if (out.yLabel.isEmpty()) {
        out.yLabel = (kind == ChartKind::Timeline) ? tr_chart("operation_type", QStringLiteral("Tipo de Operação"))
                                                   : tr_chart("quantity", QStringLiteral("Quantidade"));
    }

    const QVariantList points = fig.value(QStringLiteral("points")).toList();
    const QVariantMap colors = fig.value(QStringLiteral("op_colors")).toMap();
    const QVariantList orderHint = fig.value(QStringLiteral("operation_order")).toList();

    QHash<QString, QVector<ChartPoint>> bySeries;
    QSet<QString> seriesFound;

    for (const QVariant& item : points) {
        const QVariantMap map = item.toMap();
        const QString seriesName = map.value(QStringLiteral("tipo_operacao")).toString().trimmed();
        if (seriesName.isEmpty()) {
            continue;
        }

        ChartPoint point;
        point.color = color_or(colors.value(seriesName), QColor(QStringLiteral("#333333")));
        point.meta = map;
        point.tooltip = map.value(QStringLiteral("tooltip")).toString().trimmed();

        if (kind == ChartKind::Timeline) {
            const QDateTime dt = QDateTime::fromString(map.value(QStringLiteral("timestamp")).toString(), Qt::ISODate);
            if (!dt.isValid()) {
                continue;
            }
            point.x = static_cast<double>(dt.toMSecsSinceEpoch());
            point.label = localized_datetime_text(dt, true);
            point.tooltip = tr_chart("operation_datetime_tooltip", QStringLiteral("%1\nDate/Time: %2"))
                                .arg(seriesName)
                                .arg(point.label);
        } else if (kind == ChartKind::TemporalScatter) {
            point.x = to_double(map.value(QStringLiteral("x")));
            point.y = qMax(0.0, to_double(map.value(QStringLiteral("y"))));
            point.label = map.value(QStringLiteral("timestamp_label")).toString().trimmed();
            point.tooltip =
                QStringLiteral("%1\nArquivo: %2\nTipo: %3\nInstante: %4\nFaixa pico: %5 - %6\nConcentracao do grupo: %7")
                    .arg(seriesName)
                    .arg(map.value(QStringLiteral("arquivo")).toString().trimmed().isEmpty()
                             ? QStringLiteral("-")
                             : map.value(QStringLiteral("arquivo")).toString().trimmed())
                    .arg(map.value(QStringLiteral("tipo")).toString().trimmed().isEmpty()
                             ? QStringLiteral("-")
                             : map.value(QStringLiteral("tipo")).toString().trimmed())
                    .arg(point.label.isEmpty() ? QStringLiteral("-") : point.label)
                    .arg(map.value(QStringLiteral("bucket_label")).toString().trimmed())
                    .arg(map.value(QStringLiteral("bucket_end_label")).toString().trimmed())
                    .arg(QString::number(static_cast<int>(to_double(map.value(QStringLiteral("group_count"))))));
        } else {
            point.x = to_double(map.value(QStringLiteral("x")));
            point.y = qMax(0.0, to_double(map.value(QStringLiteral("y_mb"))));
            point.label = map.value(QStringLiteral("arquivo")).toString().trimmed();
            point.tooltip =
                QStringLiteral("%1\nArquivo: %2\nTipo: %3\nTempo: %4%5\nTimestamp: %6\nTamanho: %7 MB")
                    .arg(seriesName)
                    .arg(point.label.isEmpty() ? QStringLiteral("-") : point.label)
                    .arg(map.value(QStringLiteral("tipo")).toString().trimmed().isEmpty()
                             ? QStringLiteral("-")
                             : map.value(QStringLiteral("tipo")).toString().trimmed())
                    .arg(QString::number(point.x, 'f', 2))
                    .arg(fig.value(QStringLiteral("x_tick_suffix")).toString())
                    .arg(map.value(QStringLiteral("timestamp")).toString().trimmed())
                    .arg(QString::number(point.y, 'f', 4));
        }

        bySeries[seriesName].append(point);
        seriesFound.insert(seriesName);
    }

    QStringList orderedSeries;
    for (const QVariant& hint : orderHint) {
        const QString key = hint.toString().trimmed();
        if (!key.isEmpty() && seriesFound.contains(key) && !orderedSeries.contains(key)) {
            orderedSeries.append(key);
        }
    }
    QStringList extras = seriesFound.values();
    std::sort(extras.begin(), extras.end(), [](const QString& a, const QString& b) {
        return a.localeAwareCompare(b) < 0;
    });
    for (const QString& extra : extras) {
        if (!orderedSeries.contains(extra)) {
            orderedSeries.append(extra);
        }
    }

    for (const QString& seriesName : orderedSeries) {
        ChartSeries series;
        series.name = seriesName;
        series.role = QStringLiteral("scatter");
        series.color = color_or(colors.value(seriesName), QColor(QStringLiteral("#333333")));
        series.points = bySeries.value(seriesName);
        if (kind == ChartKind::Timeline) {
            const double yIndex = static_cast<double>(orderedSeries.indexOf(seriesName) + 1);
            for (ChartPoint& point : series.points) {
                point.y = yIndex;
            }
        }
        std::sort(series.points.begin(), series.points.end(), [](const ChartPoint& a, const ChartPoint& b) {
            return a.x < b.x;
        });
        out.series.append(series);
    }

    if (kind == ChartKind::Timeline) {
        QVariantList yCategories;
        for (int i = 0; i < orderedSeries.size(); ++i) {
            yCategories.append(orderedSeries.at(i));
        }
        out.meta.insert(QStringLiteral("xIsDateTime"), true);
        out.meta.insert(QStringLiteral("yCategories"), yCategories);
    } else if (kind == ChartKind::TemporalScatter) {
        out.meta.insert(QStringLiteral("xIsDateTime"), true);
        out.meta.insert(QStringLiteral("xMin"), fig.value(QStringLiteral("x_min")));
        out.meta.insert(QStringLiteral("xMax"), fig.value(QStringLiteral("x_max")));
        out.meta.insert(QStringLiteral("yMax"), fig.value(QStringLiteral("y_max")));
    } else {
        out.meta.insert(QStringLiteral("xTickSuffix"), fig.value(QStringLiteral("x_tick_suffix")).toString());
        out.meta.insert(QStringLiteral("yLogScale"), fig.value(QStringLiteral("y_log_scale")).toBool());
        out.meta.insert(QStringLiteral("xMin"), fig.value(QStringLiteral("x_min")));
        out.meta.insert(QStringLiteral("xMax"), fig.value(QStringLiteral("x_max")));
        out.meta.insert(QStringLiteral("yMin"), fig.value(QStringLiteral("y_min")));
        out.meta.insert(QStringLiteral("yMax"), fig.value(QStringLiteral("y_max")));
    }

    return out;
}

ChartDataSet adapt_dotplot(const QString& title, const QVariantMap& fig) {
    ChartDataSet out;
    out.kind = ChartKind::DotPlot;
    out.title = fallback_title(fig, title);
    out.xLabel = fig.value(QStringLiteral("x_label")).toString().trimmed().isEmpty()
                     ? QStringLiteral("Tipos de Arquivo")
                     : fig.value(QStringLiteral("x_label")).toString().trimmed();
    out.yLabel = fig.value(QStringLiteral("y_label")).toString().trimmed().isEmpty()
                     ? QStringLiteral("Tamanho do Arquivo (MB)")
                     : fig.value(QStringLiteral("y_label")).toString().trimmed();
    out.meta.insert(QStringLiteral("yLogScale"), fig.value(QStringLiteral("y_log_scale")).toBool());
    out.meta.insert(QStringLiteral("yMin"), fig.value(QStringLiteral("y_min")));
    out.meta.insert(QStringLiteral("yMax"), fig.value(QStringLiteral("y_max")));

    const QVariantList types = fig.value(QStringLiteral("types")).toList();
    for (const QVariant& type : types) {
        out.categories.append(type.toString());
    }

    const QVariantMap typeColors = fig.value(QStringLiteral("type_colors")).toMap();
    const QVariantMap means = fig.value(QStringLiteral("means_by_type")).toMap();
    const QVariantMap counts = fig.value(QStringLiteral("counts_by_type")).toMap();

    QHash<QString, QVector<ChartPoint>> pointsByType;
    for (const QVariant& pointVar : fig.value(QStringLiteral("points")).toList()) {
        const QVariantMap pointMap = pointVar.toMap();
        const QString type = pointMap.value(QStringLiteral("tipo")).toString().trimmed();
        const int typeIndex = out.categories.indexOf(type);
        if (type.isEmpty() || typeIndex < 0) {
            continue;
        }

        const int sequenceIndex = static_cast<int>(to_double(pointMap.value(QStringLiteral("index"))));
        const int count = qMax(1, counts.value(type).toInt());
        const double normalized = (static_cast<double>(sequenceIndex % count) / qMax(1.0, static_cast<double>(count - 1))) - 0.5;

        ChartPoint point;
        point.x = static_cast<double>(typeIndex) + (normalized * 0.42);
        point.y = qMax(0.0, to_double(pointMap.value(QStringLiteral("tamanho_mb"))));
        point.label = type;
        point.color = color_or(typeColors.value(type), QColor(QStringLiteral("#1f77b4")));
        point.tooltip =
            QStringLiteral("%1\nTamanho: %2 MB")
                .arg(type)
                .arg(QString::number(point.y, 'f', 4));
        pointsByType[type].append(point);
    }

    for (const QString& type : out.categories) {
        ChartSeries pointsSeries;
        pointsSeries.name = type;
        pointsSeries.role = QStringLiteral("scatter");
        pointsSeries.color = color_or(typeColors.value(type), QColor(QStringLiteral("#1f77b4")));
        pointsSeries.points = pointsByType.value(type);
        out.series.append(pointsSeries);

        ChartSeries meanSeries;
        meanSeries.name = QStringLiteral("%1 media").arg(type);
        meanSeries.role = QStringLiteral("mean");
        meanSeries.color = pointsSeries.color.darker(150);
        const int xIndex = out.categories.indexOf(type);
        const double mean = qMax(0.0, to_double(means.value(type)));

        ChartPoint left;
        left.x = static_cast<double>(xIndex) - 0.34;
        left.y = mean;
        left.label = type;
        left.color = meanSeries.color;

        ChartPoint right = left;
        right.x = static_cast<double>(xIndex) + 0.34;
        left.tooltip =
            QStringLiteral("%1\nMedia: %2 MB\nAmostras: %3")
                .arg(type)
                .arg(QString::number(mean, 'f', 3))
                .arg(QString::number(static_cast<int>(to_double(counts.value(type)))));
        right.tooltip = left.tooltip;
        meanSeries.points = {left, right};
        out.series.append(meanSeries);
    }

    return out;
}

ChartDataSet adapt_radar(const QString& title, const QVariantMap& fig) {
    ChartDataSet out;
    out.kind = ChartKind::Radar;
    out.title = fallback_title(fig, title);
    out.meta.insert(QStringLiteral("smoothCurve"), fig.value(QStringLiteral("smooth_curve"), true).toBool());
    out.meta.insert(QStringLiteral("maskGridUnderSeries"),
                    fig.value(QStringLiteral("mask_grid_under_series"), false).toBool());
    const QVariantList shortTypes = fig.value(QStringLiteral("types_short")).toList();
    const QVariantList longTypes = fig.value(QStringLiteral("types")).toList();
    const QVariantList labels = shortTypes.isEmpty() ? longTypes : shortTypes;
    for (const QVariant& label : labels) {
        out.categories.append(label.toString());
    }

    for (const QVariant& seriesVar : fig.value(QStringLiteral("series")).toList()) {
        const QVariantMap seriesMap = seriesVar.toMap();
        ChartSeries series;
        series.name = seriesMap.value(QStringLiteral("operacao")).toString().trimmed();
        series.role = QStringLiteral("radar");
        series.color = color_or(seriesMap.value(QStringLiteral("color")), QColor(QStringLiteral("#333333")));
        const QString quantityLabel = tr_chart("quantity", QStringLiteral("Quantidade"));
        const QString participationLabel = tr_chart("participation", QStringLiteral("Participação"));
        const QLocale locale;

        const QVariantList counts = seriesMap.value(QStringLiteral("values_count")).toList();
        const QVariantList percents = seriesMap.value(QStringLiteral("values_percent")).toList();
        for (int i = 0; i < out.categories.size(); ++i) {
            ChartPoint point;
            point.x = static_cast<double>(i);
            point.y = (i < counts.size()) ? to_double(counts.at(i)) : 0.0;
            point.y2 = (i < percents.size()) ? to_double(percents.at(i)) : 0.0;
            point.label = out.categories.at(i);
            point.color = series.color;
            point.tooltip =
                QStringLiteral("%1\n%2\n%3: %4\n%5: %6%")
                    .arg(series.name)
                    .arg(point.label)
                    .arg(quantityLabel)
                    .arg(locale.toString(std::round(point.y)))
                    .arg(participationLabel)
                    .arg(locale.toString(point.y2, 'f', 1));
            series.points.append(point);
        }
        out.series.append(series);
    }

    return out;
}

ChartDataSet adapt_boxplot_single(const QString& title, const QVariantMap& fig) {
    ChartDataSet out;
    out.kind = ChartKind::BoxPlot;
    out.title = fallback_title(fig, title);
    out.subtitle = fig.value(QStringLiteral("subtitle")).toString().trimmed();
    out.xLabel = fig.value(QStringLiteral("x_label")).toString().trimmed();
    out.yLabel = fig.value(QStringLiteral("y_label")).toString().trimmed();
    out.meta.insert(QStringLiteral("yLogScale"), fig.value(QStringLiteral("y_log_scale")).toBool());
    if (fig.contains(QStringLiteral("boxplot_layout"))) {
        out.meta.insert(QStringLiteral("boxplotLayout"), fig.value(QStringLiteral("boxplot_layout")).toString());
    }

    const QVariantMap sourceMap = normalized_chart_value_lists(fig.value(QStringLiteral("source_map")).toMap());
    const QVariantMap colorMap = normalized_chart_key_map(fig.value(QStringLiteral("color_map")).toMap());
    const QVariantList orderHint = normalized_chart_key_list(fig.value(QStringLiteral("order_hint")).toList());
    const int maxItems = fig.value(QStringLiteral("max_items"), -1).toInt();
    const QStringList ordered = ordered_keys(sourceMap, orderHint, maxItems);

    const QList<QColor> fallbackPalette = {
        QColor(QStringLiteral("#2d6cdf")),
        QColor(QStringLiteral("#8a8a8a")),
        QColor(QStringLiteral("#ff2a2a")),
        QColor(QStringLiteral("#ff7a21")),
        QColor(QStringLiteral("#e044ff")),
        QColor(QStringLiteral("#4caf50")),
        QColor(QStringLiteral("#ffd166")),
        QColor(QStringLiteral("#6dd3ce"))
    };

    for (int i = 0; i < ordered.size(); ++i) {
        const QString key = ordered.at(i);
        const QString displayLabel = translate_size_category_label(key);
        QVector<double> values;
        for (const QVariant& value : sourceMap.value(key).toList()) {
            const double number = to_double(value);
            if (fig.value(QStringLiteral("y_log_scale")).toBool()) {
                if (number > 0.0) {
                    values.append(number);
                }
            } else if (number >= 0.0) {
                values.append(number);
            }
        }

        if (values.isEmpty()) {
            continue;
        }

        std::sort(values.begin(), values.end());
        const double q1 = percentile(values, 0.25);
        const double median = percentile(values, 0.50);
        const double q3 = percentile(values, 0.75);
        const double iqr = qMax(0.0, q3 - q1);
        const double lowerFence = q1 - (iqr * 1.5);
        const double upperFence = q3 + (iqr * 1.5);

        BoxEntry box;
        box.label = displayLabel;
        box.color = color_or(colorMap.value(key), fallbackPalette.at(i % fallbackPalette.size()));
        box.sampleCount = values.size();
        box.q1 = q1;
        box.median = median;
        box.q3 = q3;

        int whiskerMinIndex = -1;
        int whiskerMaxIndex = -1;
        box.outliers.clear();
        for (int valueIndex = 0; valueIndex < values.size(); ++valueIndex) {
            const double number = values.at(valueIndex);
            if (number < lowerFence || number > upperFence) {
                box.outliers.append(number);
                continue;
            }
            if (whiskerMinIndex < 0) {
                whiskerMinIndex = valueIndex;
            }
            whiskerMaxIndex = valueIndex;
        }

        if (whiskerMinIndex < 0 || whiskerMaxIndex < 0) {
            whiskerMinIndex = 0;
            whiskerMaxIndex = values.size() - 1;
            box.outliers.clear();
        }

        box.minimum = values.at(whiskerMinIndex);
        box.maximum = values.at(whiskerMaxIndex);

        const QLocale locale;
        box.tooltip =
            QStringLiteral("%1\n%2: %3\n%4: %5\n%6: %7\n%8: %9\n%10: %11\n%12: %13")
                .arg(box.label)
                .arg(tr_chart("boxplot_samples_label", QStringLiteral("Samples")))
                .arg(locale.toString(box.sampleCount))
                .arg(tr_chart("boxplot_min_label", QStringLiteral("Min")))
                .arg(locale.toString(box.minimum, 'g', 5))
                .arg(tr_chart("boxplot_q1_label", QStringLiteral("Q1")))
                .arg(locale.toString(box.q1, 'g', 5))
                .arg(tr_chart("boxplot_median_label", QStringLiteral("Median")))
                .arg(locale.toString(box.median, 'g', 5))
                .arg(tr_chart("boxplot_q3_label", QStringLiteral("Q3")))
                .arg(locale.toString(box.q3, 'g', 5))
                .arg(tr_chart("boxplot_max_label", QStringLiteral("Max")))
                .arg(locale.toString(box.maximum, 'g', 5));
        out.boxes.append(box);
        out.categories.append(box.label);
    }

    return out;
}

}  // namespace

bool LegacyVariantChartAdapter::supports(const QString& kind) {
    static const QSet<QString> supported = {
        QStringLiteral("empty"),
        QStringLiteral("pizza"),
        QStringLiteral("barras"),
        QStringLiteral("histograma"),
        QStringLiteral("pareto"),
        QStringLiteral("linha"),
        QStringLiteral("timeline"),
        QStringLiteral("temporal_scatter"),
        QStringLiteral("scatter"),
        QStringLiteral("dotplot"),
        QStringLiteral("radar"),
        QStringLiteral("boxplot_single")
    };
    return supported.contains(kind);
}

ChartDataSet LegacyVariantChartAdapter::adapt(const QString& fallbackTitle, const QVariant& figure) {
    const QVariantMap fig = figure.toMap();
    const QString kind = fig.value(QStringLiteral("kind")).toString();

    if (fig.isEmpty() || kind == QStringLiteral("empty")) {
        return adapt_empty(fallbackTitle, fig);
    }
    if (kind == QStringLiteral("pizza")) {
        return adapt_pie(fallbackTitle, fig);
    }
    if (kind == QStringLiteral("barras")) {
        return adapt_bar_family(fallbackTitle, fig, ChartKind::Bars);
    }
    if (kind == QStringLiteral("histograma")) {
        return adapt_bar_family(fallbackTitle, fig, ChartKind::Histogram);
    }
    if (kind == QStringLiteral("pareto")) {
        return adapt_pareto(fallbackTitle, fig);
    }
    if (kind == QStringLiteral("linha")) {
        return adapt_combo_bars_line(fallbackTitle, fig);
    }
    if (kind == QStringLiteral("timeline")) {
        return adapt_timeline_like(fallbackTitle, fig, ChartKind::Timeline);
    }
    if (kind == QStringLiteral("temporal_scatter")) {
        return adapt_timeline_like(fallbackTitle, fig, ChartKind::TemporalScatter);
    }
    if (kind == QStringLiteral("scatter")) {
        return adapt_timeline_like(fallbackTitle, fig, ChartKind::Scatter);
    }
    if (kind == QStringLiteral("dotplot")) {
        return adapt_dotplot(fallbackTitle, fig);
    }
    if (kind == QStringLiteral("radar")) {
        return adapt_radar(fallbackTitle, fig);
    }
    if (kind == QStringLiteral("boxplot_single")) {
        return adapt_boxplot_single(fallbackTitle, fig);
    }

    return adapt_empty(fallbackTitle, QVariantMap{
                                        {QStringLiteral("title"), fallbackTitle},
                                        {QStringLiteral("message"),
                                         QStringLiteral("Tipo de grafico ainda nao suportado pelo backend LGPL: %1")
                                             .arg(kind)}});
}

}  // namespace source::charts_lgpl
