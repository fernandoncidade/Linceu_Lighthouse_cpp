#pragma once

#include <QColor>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QWidget>

#include "ch_07_LgplChartModel.h"

namespace source::charts_lgpl {

class WidgetChartView;
class RasterChartView;

bool supports_legacy_kind(const QString& kind);
WidgetChartView* create_chart_view(const ChartDataSet& dataSet, QWidget* parent = nullptr);
RasterChartView* create_raster_chart_view(const ChartDataSet& dataSet, QWidget* parent = nullptr);
QWidget* create_legacy_chart_widget(const QString& title, const QVariant& figure, QWidget* parent = nullptr);
QWidget* create_legacy_raster_chart_widget(const QString& title, const QVariant& figure, QWidget* parent = nullptr);

QVariantMap make_boxplot_single_figure(const QString& title,
                                       const QVariantMap& sourceMap,
                                       const QVariantList& orderHint = {},
                                       const QVariantMap& colorMap = {},
                                       bool yLogScale = false,
                                       const QString& xLabel = QString(),
                                       const QString& yLabel = QString(),
                                       int maxItems = -1);

QVariantMap make_radar_single_figure(const QString& title,
                                     const QStringList& labels,
                                     const QVector<double>& values,
                                     const QColor& color,
                                     const QString& legend = QString());

}  // namespace source::charts_lgpl
