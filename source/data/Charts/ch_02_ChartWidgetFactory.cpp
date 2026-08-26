#include "ch_02_ChartWidgetFactory.h"

#include "ch_03_LegacyVariantChartAdapter.h"
#include "ch_06_WidgetChartView.h"
#include "ch_08_RasterChartView.h"

namespace source::charts_lgpl {

bool supports_legacy_kind(const QString& kind) {
    return LegacyVariantChartAdapter::supports(kind);
}

WidgetChartView* create_chart_view(const ChartDataSet& dataSet, QWidget* parent) {
    auto* view = new WidgetChartView(parent);
    view->setDataSet(dataSet);
    view->setMinimumHeight(dataSet.kind == ChartKind::Pie || dataSet.kind == ChartKind::Radar ? 420 : 360);
    return view;
}

RasterChartView* create_raster_chart_view(const ChartDataSet& dataSet, QWidget* parent) {
    auto* view = new RasterChartView(parent);
    view->setDataSet(dataSet);
    view->setMinimumHeight(dataSet.kind == ChartKind::Pie || dataSet.kind == ChartKind::Radar ? 420 : 360);
    return view;
}

QWidget* create_legacy_chart_widget(const QString& title, const QVariant& figure, QWidget* parent) {
    const ChartDataSet dataSet = LegacyVariantChartAdapter::adapt(title, figure);
    return create_chart_view(dataSet, parent);
}

QWidget* create_legacy_raster_chart_widget(const QString& title, const QVariant& figure, QWidget* parent) {
    const ChartDataSet dataSet = LegacyVariantChartAdapter::adapt(title, figure);
    return create_raster_chart_view(dataSet, parent);
}

QVariantMap make_boxplot_single_figure(const QString& title,
                                       const QVariantMap& sourceMap,
                                       const QVariantList& orderHint,
                                       const QVariantMap& colorMap,
                                       bool yLogScale,
                                       const QString& xLabel,
                                       const QString& yLabel,
                                       int maxItems) {
    QVariantMap figure;
    figure.insert(QStringLiteral("kind"), QStringLiteral("boxplot_single"));
    figure.insert(QStringLiteral("title"), title);
    figure.insert(QStringLiteral("source_map"), sourceMap);
    figure.insert(QStringLiteral("order_hint"), orderHint);
    figure.insert(QStringLiteral("color_map"), colorMap);
    figure.insert(QStringLiteral("y_log_scale"), yLogScale);
    if (!xLabel.trimmed().isEmpty()) {
        figure.insert(QStringLiteral("x_label"), xLabel);
    }
    if (!yLabel.trimmed().isEmpty()) {
        figure.insert(QStringLiteral("y_label"), yLabel);
    }
    if (maxItems > 0) {
        figure.insert(QStringLiteral("max_items"), maxItems);
    }
    return figure;
}

QVariantMap make_radar_single_figure(const QString& title,
                                     const QStringList& labels,
                                     const QVector<double>& values,
                                     const QColor& color,
                                     const QString& legend) {
    QVariantList types;
    QVariantList shortTypes;
    QVariantList series;
    QVariantList valueCounts;
    QVariantList valuePercent;

    double total = 0.0;
    for (double value : values) {
        total += value;
    }

    for (int i = 0; i < labels.size(); ++i) {
        const QString label = labels.at(i);
        types.append(label);
        shortTypes.append(label.size() <= 14 ? label : label.left(11) + QStringLiteral("..."));
        const double value = i < values.size() ? values.at(i) : 0.0;
        valueCounts.append(value);
        valuePercent.append(total > 0.0 ? (value / total) * 100.0 : 0.0);
    }

    QVariantMap radarSeries;
    radarSeries.insert(QStringLiteral("operacao"), legend.trimmed().isEmpty() ? title : legend);
    radarSeries.insert(QStringLiteral("values_count"), valueCounts);
    radarSeries.insert(QStringLiteral("values_percent"), valuePercent);
    radarSeries.insert(QStringLiteral("color"), color.name(QColor::HexArgb));
    series.append(radarSeries);

    QVariantMap figure;
    figure.insert(QStringLiteral("kind"), QStringLiteral("radar"));
    figure.insert(QStringLiteral("title"), title);
    figure.insert(QStringLiteral("types"), types);
    figure.insert(QStringLiteral("types_short"), shortTypes);
    figure.insert(QStringLiteral("series"), series);
    return figure;
}

}  // namespace source::charts_lgpl
