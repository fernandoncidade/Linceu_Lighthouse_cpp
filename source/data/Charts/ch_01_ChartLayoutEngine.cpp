#include "ch_01_ChartLayoutEngine.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <QDateTime>
#include <QFontMetricsF>
#include <QLineF>
#include <QLocale>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QtMath>
#include <QVariantList>

namespace source::charts_lgpl {
namespace {

constexpr double kLeftMargin = 76.0;
constexpr double kTopMargin = 58.0;
constexpr double kBottomMargin = 86.0;
constexpr double kRightMargin = 34.0;
constexpr double kLegendWidth = 186.0;
constexpr double kMinLogAxisValue = 1.0e-12;
constexpr double kPi = 3.14159265358979323846;
constexpr double kHalfPi = kPi * 0.5;

QColor alpha_color(const QColor& color, int alpha) {
    QColor copy(color);
    copy.setAlpha(alpha);
    return copy;
}

ColoredVertex make_vertex(double x, double y, const QColor& color) {
    return ColoredVertex{
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(color.redF()),
        static_cast<float>(color.greenF()),
        static_cast<float>(color.blueF()),
        static_cast<float>(color.alphaF())};
}

void append_triangle(QVector<ColoredVertex>& out, const QPointF& a, const QPointF& b, const QPointF& c,
                     const QColor& color) {
    out.append(make_vertex(a.x(), a.y(), color));
    out.append(make_vertex(b.x(), b.y(), color));
    out.append(make_vertex(c.x(), c.y(), color));
}

void append_line(QVector<ColoredVertex>& out, const QPointF& a, const QPointF& b, const QColor& color) {
    out.append(make_vertex(a.x(), a.y(), color));
    out.append(make_vertex(b.x(), b.y(), color));
}

void append_rect(QVector<ColoredVertex>& out, const QRectF& rect, const QColor& color) {
    append_triangle(out, rect.topLeft(), rect.topRight(), rect.bottomRight(), color);
    append_triangle(out, rect.topLeft(), rect.bottomRight(), rect.bottomLeft(), color);
}

void append_vertical_strip(QVector<ColoredVertex>& out, const QRectF& bounds, double x, const QColor& color,
                           double thickness = 1.0) {
    const QRectF strip(x - (thickness * 0.5), bounds.top(), thickness, bounds.height());
    append_rect(out, strip, color);
}

void append_horizontal_strip(QVector<ColoredVertex>& out, const QRectF& bounds, double y, const QColor& color,
                             double thickness = 1.0) {
    const QRectF strip(bounds.left(), y - (thickness * 0.5), bounds.width(), thickness);
    append_rect(out, strip, color);
}

void append_rect_outline(QVector<ColoredVertex>& out, const QRectF& rect, const QColor& color) {
    append_line(out, rect.topLeft(), rect.topRight(), color);
    append_line(out, rect.topRight(), rect.bottomRight(), color);
    append_line(out, rect.bottomRight(), rect.bottomLeft(), color);
    append_line(out, rect.bottomLeft(), rect.topLeft(), color);
}

void append_polygon_fan(QVector<ColoredVertex>& out, const QVector<QPointF>& polygon, const QColor& color) {
    if (polygon.size() < 3) {
        return;
    }
    const QPointF origin = polygon.first();
    for (int i = 1; i + 1 < polygon.size(); ++i) {
        append_triangle(out, origin, polygon.at(i), polygon.at(i + 1), color);
    }
}

void append_polygon_fan_from_center(QVector<ColoredVertex>& out, const QPointF& center, const QVector<QPointF>& polygon,
                                    const QColor& color) {
    if (polygon.size() < 3) {
        return;
    }
    for (int i = 0; i < polygon.size(); ++i) {
        append_triangle(out, center, polygon.at(i), polygon.at((i + 1) % polygon.size()), color);
    }
}

void append_polygon_outline(QVector<ColoredVertex>& out, const QVector<QPointF>& polygon, const QColor& color,
                            bool closed = true) {
    if (polygon.size() < 2) {
        return;
    }
    for (int i = 0; i + 1 < polygon.size(); ++i) {
        append_line(out, polygon.at(i), polygon.at(i + 1), color);
    }
    if (closed) {
        append_line(out, polygon.constLast(), polygon.constFirst(), color);
    }
}

void append_circle(QVector<ColoredVertex>& fills, QVector<ColoredVertex>& lines, const QPointF& center,
                   double radius, const QColor& fill, const QColor& stroke, int segments = 18) {
    QVector<QPointF> polygon;
    segments = qMax(segments, static_cast<int>(std::ceil(radius * 0.9)));
    polygon.reserve(segments);
    for (int i = 0; i < segments; ++i) {
        const double angle = (static_cast<double>(i) / static_cast<double>(segments)) * (kPi * 2.0);
        polygon.append(QPointF(center.x() + (std::cos(angle) * radius), center.y() + (std::sin(angle) * radius)));
    }
    append_polygon_fan(fills, polygon, fill);
    append_polygon_outline(lines, polygon, stroke);
}

void append_circle_direct(QVector<ColoredVertex>& fills, QVector<ColoredVertex>& lines, const QPointF& center,
                          double radius, const QColor& fill, const QColor& stroke, int segments = 12) {
    segments = qMax(6, segments);
    QPointF previous(center.x() + radius, center.y());
    for (int i = 1; i <= segments; ++i) {
        const double angle = (static_cast<double>(i) / static_cast<double>(segments)) * (kPi * 2.0);
        const QPointF current(center.x() + (std::cos(angle) * radius), center.y() + (std::sin(angle) * radius));
        append_triangle(fills, center, previous, current, fill);
        append_line(lines, previous, current, stroke);
        previous = current;
    }
}

void append_circle_outline(QVector<ColoredVertex>& lines, const QPointF& center, double radius, const QColor& stroke,
                           int segments = 72) {
    QVector<QPointF> polygon;
    polygon.reserve(segments);
    for (int i = 0; i < segments; ++i) {
        const double angle = (static_cast<double>(i) / static_cast<double>(segments)) * (kPi * 2.0);
        polygon.append(QPointF(center.x() + (std::cos(angle) * radius), center.y() + (std::sin(angle) * radius)));
    }
    append_polygon_outline(lines, polygon, stroke);
}

void append_line_band(QVector<ColoredVertex>& fills, const QPointF& a, const QPointF& b, double thickness,
                      const QColor& color) {
    const QLineF line(a, b);
    if (line.length() <= 0.0 || thickness <= 0.0) {
        return;
    }

    const double halfThickness = thickness * 0.5;
    const QPointF normal(-(line.dy() / line.length()) * halfThickness, (line.dx() / line.length()) * halfThickness);
    const QPointF p1 = a + normal;
    const QPointF p2 = b + normal;
    const QPointF p3 = b - normal;
    const QPointF p4 = a - normal;
    append_triangle(fills, p1, p2, p3, color);
    append_triangle(fills, p1, p3, p4, color);
}

void append_circle_band(QVector<ColoredVertex>& fills, const QPointF& center, double radius, double thickness,
                        const QColor& color, int segments = 96) {
    if (radius <= 0.0 || thickness <= 0.0) {
        return;
    }

    const double halfThickness = thickness * 0.5;
    const double innerRadius = qMax(0.0, radius - halfThickness);
    const double outerRadius = radius + halfThickness;
    QVector<QPointF> outer;
    QVector<QPointF> inner;
    outer.reserve(segments);
    inner.reserve(segments);
    for (int i = 0; i < segments; ++i) {
        const double angle = (static_cast<double>(i) / static_cast<double>(segments)) * (kPi * 2.0);
        const QPointF dir(std::cos(angle), std::sin(angle));
        outer.append(center + QPointF(dir.x() * outerRadius, dir.y() * outerRadius));
        inner.append(center + QPointF(dir.x() * innerRadius, dir.y() * innerRadius));
    }

    for (int i = 0; i < segments; ++i) {
        const int next = (i + 1) % segments;
        append_triangle(fills, outer.at(i), outer.at(next), inner.at(next), color);
        append_triangle(fills, outer.at(i), inner.at(next), inner.at(i), color);
    }
}

void append_donut_slice(QVector<ColoredVertex>& fills, QVector<ColoredVertex>& lines, const QPointF& center,
                        double innerRadius, double outerRadius, double startAngle, double sweepAngle,
                        const QColor& fill, const QColor& stroke) {
    const double arcLength = (kPi * 2.0 * qMax(outerRadius, 1.0)) * (std::abs(sweepAngle) / 360.0);
    const int segments =
        qBound(18, static_cast<int>(std::ceil(qMax(arcLength / 4.0, std::abs(sweepAngle) / 2.0))), 240);
    QVector<QPointF> outer;
    QVector<QPointF> inner;
    outer.reserve(segments + 1);
    inner.reserve(segments + 1);

    for (int i = 0; i <= segments; ++i) {
        const double t = static_cast<double>(i) / static_cast<double>(segments);
        const double angle = qDegreesToRadians(startAngle + (sweepAngle * t));
        const QPointF dir(std::cos(angle), -std::sin(angle));
        outer.append(center + QPointF(dir.x() * outerRadius, dir.y() * outerRadius));
        inner.append(center + QPointF(dir.x() * innerRadius, dir.y() * innerRadius));
    }

    for (int i = 0; i < segments; ++i) {
        append_triangle(fills, outer.at(i), outer.at(i + 1), inner.at(i + 1), fill);
        append_triangle(fills, outer.at(i), inner.at(i + 1), inner.at(i), fill);
        append_line(lines, outer.at(i), outer.at(i + 1), stroke);
        append_line(lines, inner.at(i), inner.at(i + 1), stroke);
    }
    append_line(lines, outer.first(), inner.first(), stroke);
    append_line(lines, outer.constLast(), inner.constLast(), stroke);
}

double catmull_rom(double p0, double p1, double p2, double p3, double t) {
    const double t2 = t * t;
    const double t3 = t2 * t;
    return 0.5 * ((2.0 * p1) + ((-p0 + p2) * t) + ((2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3) * t2) +
                  ((-p0 + 3.0 * p1 - 3.0 * p2 + p3) * t3));
}

double nice_ceil(double value) {
    if (!std::isfinite(value) || value <= 0.0) {
        return 1.0;
    }
    const double exponent = std::pow(10.0, std::floor(std::log10(value)));
    const double normalized = value / exponent;
    double base = 1.0;
    if (normalized <= 1.0) base = 1.0;
    else if (normalized <= 2.0) base = 2.0;
    else if (normalized <= 5.0) base = 5.0;
    else base = 10.0;
    return base * exponent;
}

double nice_log_floor(double value) {
    if (!std::isfinite(value) || value <= 0.0) {
        return kMinLogAxisValue;
    }

    const double exponent = std::pow(10.0, std::floor(std::log10(value)));
    const double normalized = value / exponent;
    double base = 0.5;
    if (normalized <= 1.0) base = 0.5;
    else if (normalized <= 2.0) base = 1.0;
    else if (normalized <= 5.0) base = 2.0;
    else base = 5.0;
    return qMax(kMinLogAxisValue, base * exponent);
}

double nice_log_ceil(double value) {
    if (!std::isfinite(value) || value <= 0.0) {
        return 1.0;
    }

    const double exponent = std::pow(10.0, std::floor(std::log10(value)));
    const double normalized = value / exponent;
    double base = 1.0;
    if (normalized <= 1.0) base = 1.0;
    else if (normalized <= 2.0) base = 2.0;
    else if (normalized <= 5.0) base = 5.0;
    else base = 10.0;
    return base * exponent;
}

QPair<double, double> padded_boxplot_y_range(double minValue, double maxValue, bool logScale) {
    if (!std::isfinite(minValue) || !std::isfinite(maxValue) || minValue >= maxValue) {
        return {logScale ? 0.01 : 0.0, logScale ? 100.0 : 1.0};
    }

    if (logScale) {
        const double safeMin = qMax(kMinLogAxisValue, minValue);
        const double safeMax = qMax(safeMin * 1.2, maxValue);
        const double paddedMin =
            qMax(kMinLogAxisValue, std::pow(10.0, std::floor(std::log10(safeMin)) - 1.0));
        const double paddedMax = qMax(nice_log_ceil(safeMax * 1.08), paddedMin * 10.0);
        return {paddedMin, paddedMax};
    }

    const double span = maxValue - minValue;
    const double reference = qMax(1.0, qMax(std::abs(minValue), std::abs(maxValue)));
    const double padding = qMax(span * 0.08, reference * 0.03);
    const double paddedMin = (minValue >= 0.0) ? qMax(0.0, minValue - padding) : (minValue - padding);
    const double paddedMax = qMax(paddedMin + 1.0, nice_ceil(maxValue + padding));
    return {paddedMin, paddedMax};
}

double rounded_count_axis_max(double value);

double rounded_count_axis_precision(double value) {
    if (!std::isfinite(value) || value <= 0.0) {
        return 1.0;
    }

    if (value >= 10000.0) {
        return 1000.0;
    }
    if (value >= 1000.0) {
        return 100.0;
    }
    if (value >= 100.0) {
        return 10.0;
    }
    if (value >= 20.0) {
        return 5.0;
    }
    if (value >= 10.0) {
        return 2.0;
    }
    return 1.0;
}

QPair<double, double> padded_count_boxplot_y_range(double minValue, double maxValue) {
    if (!std::isfinite(minValue) || !std::isfinite(maxValue)) {
        return {0.0, 1.0};
    }

    const double safeMin = qMax(0.0, minValue);
    const double safeMax = qMax(safeMin, maxValue);
    const double precision = rounded_count_axis_precision(qMax(safeMax, safeMax - safeMin));

    if (safeMax <= safeMin) {
        return {qMax(0.0, safeMin - precision), rounded_count_axis_max(safeMax + precision)};
    }

    const double span = safeMax - safeMin;
    const double padding = qMax(span * 0.08, precision * 0.75);
    double paddedMin = std::floor(qMax(0.0, safeMin - padding) / precision) * precision;
    double paddedMax = rounded_count_axis_max(safeMax + padding);

    if (paddedMin >= safeMin && paddedMin >= precision) {
        paddedMin -= precision;
    }
    if (paddedMax <= safeMax) {
        paddedMax += precision;
    }

    return {qMax(0.0, paddedMin), qMax(paddedMin + precision, paddedMax)};
}

double rotated_label_band_height(const QStringList& labels, qreal fontSize = 9.0, qreal angleDegrees = 45.0) {
    if (labels.isEmpty()) {
        return 0.0;
    }

    QFont font;
    font.setPointSizeF(fontSize);
    const QFontMetricsF metrics(font);
    qreal maxWidth = 0.0;
    for (const QString& rawLabel : labels) {
        const QString label = rawLabel.trimmed();
        if (!label.isEmpty()) {
            maxWidth = qMax(maxWidth, metrics.horizontalAdvance(label));
        }
    }

    if (maxWidth <= 0.0) {
        return metrics.height();
    }

    const qreal radians = qDegreesToRadians(std::abs(angleDegrees));
    return (maxWidth * std::sin(radians)) + (metrics.height() * std::cos(radians));
}

double rounded_count_axis_max(double value) {
    if (!std::isfinite(value) || value <= 0.0) {
        return 1.0;
    }

    const double precision = rounded_count_axis_precision(value);
    double upper = std::ceil(value / precision) * precision;
    if (upper <= value) {
        upper += precision;
    }
    return upper;
}

double y_axis_title_band_width(const QString& label, qreal fontSize = 10.0) {
    const QString trimmed = label.trimmed();
    if (trimmed.isEmpty()) {
        return 0.0;
    }

    QFont font;
    font.setPointSizeF(fontSize);
    font.setBold(true);
    const QFontMetricsF metrics(font);
    return qBound(28.0, (metrics.horizontalAdvance(trimmed) * 0.5) + 18.0, 96.0);
}

double secondary_axis_tick_band(const ChartDataSet& dataSet) {
    return (dataSet.kind == ChartKind::Pareto) ? 50.0 : 58.0;
}

double secondary_axis_title_footprint(const ChartDataSet& dataSet) {
    if (dataSet.y2Label.trimmed().isEmpty()) {
        return 0.0;
    }

    QFont font;
    font.setPointSizeF(10.0);
    font.setBold(true);
    const QFontMetricsF metrics(font);
    return qBound(20.0, metrics.height() + 10.0, 30.0);
}

double secondary_axis_title_center_offset(const ChartDataSet& dataSet) {
    return secondary_axis_tick_band(dataSet) + ((dataSet.kind == ChartKind::Pareto) ? 18.0 : 24.0);
}

double secondary_axis_band_width(const ChartDataSet& dataSet) {
    const QString trimmed = dataSet.y2Label.trimmed();
    if (trimmed.isEmpty()) {
        return 0.0;
    }

    const double titleFootprint = secondary_axis_title_footprint(dataSet);
    const double centerOffset = secondary_axis_title_center_offset(dataSet);
    return centerOffset + (titleFootprint * 0.5) + ((dataSet.kind == ChartKind::Pareto) ? 4.0 : 12.0);
}

double legend_band_width(const ChartDataSet& dataSet) {
    const bool hasLegend = dataSet.kind != ChartKind::BoxPlot && dataSet.kind != ChartKind::Empty &&
                           !dataSet.meta.value(QStringLiteral("hideLegend")).toBool() && !dataSet.series.isEmpty();
    if (!hasLegend) {
        return 0.0;
    }

    if (dataSet.kind != ChartKind::ComboBarsLine) {
        return kLegendWidth;
    }

    QFont legendFont;
    legendFont.setPointSizeF(9.5);
    const QFontMetricsF legendMetrics(legendFont);
    qreal longestLabel = 0.0;
    for (const ChartSeries& series : dataSet.series) {
        const QString label = series.meta.value(QStringLiteral("legendLabel")).toString().trimmed().isEmpty()
                                  ? series.name.trimmed()
                                  : series.meta.value(QStringLiteral("legendLabel")).toString().trimmed();
        if (!label.isEmpty()) {
            longestLabel = qMax(longestLabel, legendMetrics.horizontalAdvance(label));
        }
    }

    if (longestLabel <= 0.0) {
        return kLegendWidth;
    }

    return qBound(186.0, longestLabel + 34.0, 360.0);
}

bool combo_bars_line_uses_rotated_dates(const ChartDataSet& dataSet) {
    return dataSet.kind == ChartKind::ComboBarsLine && dataSet.categories.size() > 7;
}

bool dotplot_uses_rotated_labels(const ChartDataSet& dataSet) {
    if (dataSet.kind != ChartKind::DotPlot) {
        return false;
    }

    if (dataSet.categories.size() > 8) {
        return true;
    }

    for (const QString& label : dataSet.categories) {
        if (label.trimmed().size() > 8) {
            return true;
        }
    }

    return false;
}

bool boxplot_uses_rotated_labels(const ChartDataSet& dataSet) {
    if (dataSet.kind != ChartKind::BoxPlot) {
        return false;
    }

    if (dataSet.boxes.size() > 6) {
        return true;
    }

    for (const BoxEntry& box : dataSet.boxes) {
        if (box.label.trimmed().size() > 8) {
            return true;
        }
    }

    return false;
}

bool boxplot_uses_compact_hour_layout(const ChartDataSet& dataSet) {
    return dataSet.kind == ChartKind::BoxPlot &&
           dataSet.meta.value(QStringLiteral("boxplotLayout")).toString() == QStringLiteral("hour_of_day_compact");
}

double boxplot_label_band_height(const ChartDataSet& dataSet) {
    if (dataSet.kind != ChartKind::BoxPlot || dataSet.boxes.isEmpty()) {
        return 0.0;
    }

    QStringList labels;
    labels.reserve(dataSet.boxes.size());
    for (const BoxEntry& box : dataSet.boxes) {
        labels.append(box.label);
    }

    if (boxplot_uses_rotated_labels(dataSet)) {
        return qBound(88.0, rotated_label_band_height(labels, 9.0, 45.0) + 18.0, 170.0);
    }

    QFont categoryFont;
    categoryFont.setPointSizeF(9.0);
    const QFontMetricsF categoryMetrics(categoryFont);
    return qBound(28.0, categoryMetrics.height() + 12.0, 38.0);
}

double combo_bars_line_label_band_height(const ChartDataSet& dataSet) {
    if (dataSet.kind != ChartKind::ComboBarsLine || dataSet.categories.isEmpty()) {
        return 0.0;
    }

    if (combo_bars_line_uses_rotated_dates(dataSet)) {
        return qBound(68.0, rotated_label_band_height(dataSet.categories, 9.0, 45.0) + 14.0, 144.0);
    }

    QFont categoryFont;
    categoryFont.setPointSizeF(9.0);
    const QFontMetricsF categoryMetrics(categoryFont);
    return qBound(26.0, categoryMetrics.height() + 10.0, 34.0);
}

double dotplot_label_band_height(const ChartDataSet& dataSet) {
    if (dataSet.kind != ChartKind::DotPlot || dataSet.categories.isEmpty()) {
        return 0.0;
    }

    if (dotplot_uses_rotated_labels(dataSet)) {
        return qBound(72.0, rotated_label_band_height(dataSet.categories, 9.0, 42.0) + 14.0, 144.0);
    }

    QFont categoryFont;
    categoryFont.setPointSizeF(9.0);
    const QFontMetricsF categoryMetrics(categoryFont);
    return qBound(26.0, categoryMetrics.height() + 10.0, 34.0);
}

QRectF plot_rect_for(const ChartDataSet& dataSet, const QSizeF& size) {
    const bool hasLegend = dataSet.kind != ChartKind::BoxPlot && dataSet.kind != ChartKind::Empty;
    const double topMargin = kTopMargin + ((dataSet.kind == ChartKind::Histogram) ? 8.0 : 0.0);
    double leftMargin = kLeftMargin;
    double bottomMargin = kBottomMargin;

    if (!dataSet.yLabel.trimmed().isEmpty()) {
        QFont axisTitleFont;
        axisTitleFont.setPointSizeF(10.0);
        axisTitleFont.setBold(true);
        const QFontMetricsF axisTitleMetrics(axisTitleFont);
        const double yTitleWidth = axisTitleMetrics.horizontalAdvance(dataSet.yLabel.trimmed()) + 38.0;
        leftMargin = qMax(leftMargin, yTitleWidth);
    }

    const QVariantList yCategories = dataSet.meta.value(QStringLiteral("yCategories")).toList();
    if (!yCategories.isEmpty()) {
        int maxLen = 0;
        for (const QVariant& item : yCategories) {
            maxLen = qMax(maxLen, item.toString().trimmed().size());
        }
        const double categoryBand = qBound(112.0, 42.0 + (maxLen * 7.2), 220.0);
        const double titleBand =
            dataSet.yLabel.trimmed().isEmpty() ? 0.0 : (y_axis_title_band_width(dataSet.yLabel, 10.0) + 12.0);
        leftMargin = qMax(leftMargin, qMin(320.0, categoryBand + titleBand));
    }

    QStringList xLabels = dataSet.categories;
    if (dataSet.kind == ChartKind::BoxPlot) {
        xLabels.clear();
        xLabels.reserve(dataSet.boxes.size());
        for (const BoxEntry& box : dataSet.boxes) {
            xLabels.append(box.label);
        }
        leftMargin = qMax(leftMargin, 96.0);
    }
    int maxXLen = 0;
    for (const QString& label : xLabels) {
        maxXLen = qMax(maxXLen, label.trimmed().size());
    }
    if (dataSet.kind == ChartKind::BoxPlot) {
        const double labelBand = boxplot_label_band_height(dataSet);
        const double titleBand = dataSet.xLabel.trimmed().isEmpty() ? 32.0 : 56.0;
        bottomMargin = qMax(bottomMargin, qBound(148.0, labelBand + titleBand + 30.0, 252.0));
    } else if (dataSet.kind == ChartKind::Bars && !xLabels.isEmpty()) {
        const double labelBand = rotated_label_band_height(xLabels, 9.0, 45.0);
        const double titleBand = dataSet.xLabel.trimmed().isEmpty() ? 32.0 : 56.0;
        bottomMargin = qMax(bottomMargin, qBound(132.0, labelBand + titleBand + 22.0, 220.0));
    } else if (dataSet.kind == ChartKind::Pareto && !xLabels.isEmpty()) {
        const double titleBand = dataSet.xLabel.trimmed().isEmpty() ? 32.0 : 56.0;
        bottomMargin = qMax(bottomMargin, qBound(96.0, 28.0 + titleBand + 18.0, 156.0));
    } else if (dataSet.kind == ChartKind::ComboBarsLine && !xLabels.isEmpty()) {
        const double labelBand = combo_bars_line_label_band_height(dataSet);
        const double titleBand = dataSet.xLabel.trimmed().isEmpty() ? 32.0 : 56.0;
        bottomMargin = qMax(bottomMargin,
                            combo_bars_line_uses_rotated_dates(dataSet)
                                ? qBound(120.0, labelBand + titleBand + 18.0, 196.0)
                                : qBound(102.0, labelBand + titleBand + 18.0, 132.0));
    } else if (dataSet.kind == ChartKind::DotPlot && !xLabels.isEmpty()) {
        const double labelBand = dotplot_label_band_height(dataSet);
        const double titleBand = dataSet.xLabel.trimmed().isEmpty() ? 32.0 : 56.0;
        bottomMargin = qMax(bottomMargin,
                            dotplot_uses_rotated_labels(dataSet)
                                ? qBound(120.0, labelBand + titleBand + 18.0, 196.0)
                                : qBound(96.0, labelBand + titleBand + 18.0, 132.0));
    } else if (maxXLen > 11 || xLabels.size() > 10) {
        bottomMargin = qMax(bottomMargin, 104.0);
    }

    const double legendSpace = hasLegend ? legend_band_width(dataSet) : 0.0;
    const double rightAxisBand = secondary_axis_band_width(dataSet);
    const double width = qMax(120.0, size.width() - leftMargin - kRightMargin - legendSpace - rightAxisBand);
    const double height = qMax(100.0, size.height() - topMargin - bottomMargin);
    return QRectF(leftMargin, topMargin, width, height);
}

int active_series(const ChartViewState& state) {
    if (state.hoveredLegend >= 0 && state.hoveredElement >= 0) {
        return -1;
    }
    if (state.selectedSeries >= 0 && state.selectedElement >= 0) {
        return -1;
    }
    if (state.hoveredLegend >= 0) {
        return state.hoveredLegend;
    }
    if (state.hoveredSeries >= 0) {
        return state.hoveredSeries;
    }
    if (state.selectedSeries >= 0) {
        return state.selectedSeries;
    }
    return -1;
}

bool active_element(const ChartViewState& state, int seriesIndex, int elementIndex) {
    if (seriesIndex == state.hoveredSeries && elementIndex == state.hoveredElement) {
        return true;
    }
    if (seriesIndex == state.selectedSeries && elementIndex == state.selectedElement) {
        return true;
    }
    return false;
}

bool has_focused_element(const ChartViewState& state) {
    return (state.hoveredSeries >= 0 && state.hoveredElement >= 0) ||
           (state.selectedSeries >= 0 && state.selectedElement >= 0);
}

bool has_selected_element(const ChartViewState& state) {
    return state.selectedSeries >= 0 && state.selectedElement >= 0;
}

bool is_selected_element(const ChartViewState& state, int seriesIndex, int elementIndex) {
    return seriesIndex == state.selectedSeries && elementIndex == state.selectedElement;
}

int focused_series(const ChartViewState& state) {
    if (state.hoveredSeries >= 0 && state.hoveredElement >= 0) {
        return state.hoveredSeries;
    }
    if (state.selectedSeries >= 0 && state.selectedElement >= 0) {
        return state.selectedSeries;
    }
    return -1;
}

int focused_element(const ChartViewState& state) {
    if (state.hoveredSeries >= 0 && state.hoveredElement >= 0) {
        return state.hoveredElement;
    }
    if (state.selectedSeries >= 0 && state.selectedElement >= 0) {
        return state.selectedElement;
    }
    return -1;
}

bool is_grayish_color(const QColor& color) {
    const int maxChannel = qMax(color.red(), qMax(color.green(), color.blue()));
    const int minChannel = qMin(color.red(), qMin(color.green(), color.blue()));
    return (maxChannel - minChannel) <= 18;
}

QColor blend_color(const QColor& base, const QColor& target, qreal amount) {
    const qreal t = qBound<qreal>(0.0, amount, 1.0);
    QColor blended;
    blended.setRgbF(base.redF() + ((target.redF() - base.redF()) * t),
                    base.greenF() + ((target.greenF() - base.greenF()) * t),
                    base.blueF() + ((target.blueF() - base.blueF()) * t),
                    base.alphaF());
    return blended;
}

QColor inactive_chart_color(const QColor& base, qreal alphaFactor) {
    if (is_grayish_color(base)) {
        return blend_color(base, QColor(QStringLiteral("#c8ced6")), 0.58);
    }

    QColor color = base;
    color.setAlphaF(color.alphaF() * alphaFactor);
    return color;
}

QColor active_chart_color(const QColor& base, int lighterFactor) {
    if (is_grayish_color(base)) {
        return base;
    }
    return base.lighter(lighterFactor);
}

QColor selected_plot_element_color(const QColor& base) {
    if (is_grayish_color(base)) {
        return blend_color(base, QColor(QStringLiteral("#4b5563")), 0.18);
    }
    return base.darker(116);
}

QColor unselected_plot_element_color(const QColor& base) {
    if (is_grayish_color(base)) {
        return blend_color(base, QColor(QStringLiteral("#f8fafc")), 0.42);
    }
    return blend_color(base, QColor(QStringLiteral("#ffffff")), 0.26);
}

int inactive_point_alpha(const QColor& color) {
    return is_grayish_color(color) ? 150 : 90;
}

QColor series_color(const QColor& base, const ChartViewState& state, int seriesIndex, int elementIndex = -1) {
    QColor color = base;
    const int active = active_series(state);
    if (active >= 0 && active != seriesIndex) {
        color = inactive_chart_color(color, 0.22);
    }
    if (active_element(state, seriesIndex, elementIndex) || active == seriesIndex) {
        color = active_chart_color(color, 115);
    }
    return color;
}

QColor element_color(const QColor& base, const ChartViewState& state, int seriesIndex, int elementIndex) {
    QColor color = base;
    if (has_focused_element(state)) {
        if (seriesIndex != focused_series(state) || elementIndex != focused_element(state)) {
            return inactive_chart_color(color, 0.25);
        }
        return active_chart_color(color, 118);
    }
    return series_color(base, state, seriesIndex, elementIndex);
}

QColor plotted_element_fill_color(const QColor& base, const ChartViewState& state, int seriesIndex, int elementIndex) {
    if (has_selected_element(state)) {
        return is_selected_element(state, seriesIndex, elementIndex)
                   ? selected_plot_element_color(base)
                   : unselected_plot_element_color(base);
    }
    return element_color(base, state, seriesIndex, elementIndex);
}

QColor line_color(const QColor& base, const ChartViewState& state, int seriesIndex) {
    QColor color = base;
    const int active = active_series(state);
    if (active >= 0 && active != seriesIndex) {
        color = inactive_chart_color(color, 0.28);
    }
    if (active == seriesIndex) {
        color = color.darker(110);
    }
    return color;
}

void add_title_block(ChartScene& scene, const ChartDataSet& dataSet, const QSizeF& size) {
    QFont titleFont;
    titleFont.setPointSizeF(16.0);
    titleFont.setBold(true);
    const QFontMetricsF titleMetrics(titleFont);
    const double titleTop = 12.0;
    const double titleHeight = qMax(30.0, titleMetrics.height() + 8.0);
    scene.labels.append(OverlayLabel{
        QRectF(18.0, titleTop, size.width() - 36.0, titleHeight),
        dataSet.title,
        QColor(QStringLiteral("#111827")),
        16.0,
        Qt::AlignHCenter | Qt::AlignVCenter,
        0.0,
        true});

    if (!dataSet.subtitle.trimmed().isEmpty()) {
        QFont subtitleFont;
        subtitleFont.setPointSizeF(10.0);
        const QFontMetricsF subtitleMetrics(subtitleFont);
        const double subtitleTop = titleTop + titleHeight - 4.0;
        const double subtitleHeight = qMax(18.0, subtitleMetrics.height() + 6.0);
        scene.labels.append(OverlayLabel{
            QRectF(18.0, subtitleTop, size.width() - 36.0, subtitleHeight),
            dataSet.subtitle,
            QColor(QStringLiteral("#4b5563")),
            10.0,
            Qt::AlignHCenter | Qt::AlignVCenter,
            0.0,
            false});
    }
}

void add_axis_titles(ChartScene& scene, const ChartDataSet& dataSet) {
    if (!dataSet.xLabel.trimmed().isEmpty()) {
        double xTitleTop = scene.plotRect.bottom() + 40.0;
        if (dataSet.kind == ChartKind::Bars && !dataSet.categories.isEmpty()) {
            const double labelBand =
                qBound(72.0, rotated_label_band_height(dataSet.categories, 9.0, 45.0) + 14.0, 144.0);
            xTitleTop = scene.plotRect.bottom() + labelBand + 12.0;
        } else if (dataSet.kind == ChartKind::BoxPlot && !dataSet.boxes.isEmpty()) {
            xTitleTop = scene.plotRect.bottom() + boxplot_label_band_height(dataSet) + 12.0;
        } else if (dataSet.kind == ChartKind::Pareto && !dataSet.categories.isEmpty()) {
            xTitleTop = scene.plotRect.bottom() + 36.0;
        } else if (dataSet.kind == ChartKind::ComboBarsLine && !dataSet.categories.isEmpty()) {
            xTitleTop = scene.plotRect.bottom() + combo_bars_line_label_band_height(dataSet) + 12.0;
        } else if (dataSet.kind == ChartKind::DotPlot && !dataSet.categories.isEmpty()) {
            xTitleTop = scene.plotRect.bottom() + dotplot_label_band_height(dataSet) + 12.0;
        }
        scene.labels.append(OverlayLabel{
            QRectF(scene.plotRect.left(), xTitleTop, scene.plotRect.width(), 24.0),
            dataSet.xLabel,
            QColor(QStringLiteral("#374151")),
            10.0,
            Qt::AlignHCenter | Qt::AlignVCenter,
            0.0,
            true});
    }
    if (!dataSet.yLabel.trimmed().isEmpty()) {
        QFont axisTitleFont;
        axisTitleFont.setPointSizeF(10.0);
        axisTitleFont.setBold(true);
        const QFontMetricsF axisTitleMetrics(axisTitleFont);
        const double desiredWidth = axisTitleMetrics.horizontalAdvance(dataSet.yLabel.trimmed()) + 12.0;
        const bool hasYCategories = !dataSet.meta.value(QStringLiteral("yCategories")).toList().isEmpty();
        const double labelWidth = qMax(34.0, desiredWidth);
        const double axisGap = (!hasYCategories && dataSet.kind == ChartKind::Pareto) ? 18.0 : 10.0;
        const double labelLeft = hasYCategories ? 6.0 : qMax(6.0, scene.plotRect.left() - labelWidth - axisGap);
        scene.labels.append(OverlayLabel{
            QRectF(labelLeft, scene.plotRect.top(), labelWidth, scene.plotRect.height()),
            dataSet.yLabel,
            QColor(QStringLiteral("#374151")),
            10.0,
            Qt::AlignHCenter | Qt::AlignVCenter,
            -90.0,
            true});
    }
    if (!dataSet.y2Label.trimmed().isEmpty()) {
        QFont axisTitleFont;
        axisTitleFont.setPointSizeF(10.0);
        axisTitleFont.setBold(true);
        const QFontMetricsF axisTitleMetrics(axisTitleFont);
        const double tickBand = secondary_axis_tick_band(dataSet);
        const double labelWidth = qBound(42.0, axisTitleMetrics.horizontalAdvance(dataSet.y2Label.trimmed()) + 14.0, 160.0);
        const double centerOffset = secondary_axis_title_center_offset(dataSet);
        scene.labels.append(OverlayLabel{
            QRectF(scene.plotRect.right() + centerOffset - (labelWidth * 0.5),
                   scene.plotRect.top(), labelWidth, scene.plotRect.height()),
            dataSet.y2Label,
            QColor(QStringLiteral("#374151")),
            10.0,
            Qt::AlignHCenter | Qt::AlignVCenter,
            90.0,
            true});
    }
}

void add_legend(ChartScene& scene, const ChartDataSet& dataSet, const ChartViewState& state) {
    if (dataSet.kind == ChartKind::Empty || dataSet.kind == ChartKind::BoxPlot ||
        dataSet.meta.value(QStringLiteral("hideLegend")).toBool() ||
        dataSet.series.isEmpty()) {
        return;
    }

    struct PendingLegendEntry {
        QString label;
        QString tooltip;
        QColor color;
        int seriesIndex = -1;
        int elementIndex = -1;
    };

    QVector<PendingLegendEntry> pending;
    const bool legendFromPoints = dataSet.meta.value(QStringLiteral("legendFromPoints")).toBool();
    if (legendFromPoints) {
        const ChartSeries& primary = dataSet.series.constFirst();
        pending.reserve(primary.points.size() + qMax(0, dataSet.series.size() - 1));
        for (int pointIndex = 0; pointIndex < primary.points.size(); ++pointIndex) {
            const ChartPoint& point = primary.points.at(pointIndex);
            pending.append(PendingLegendEntry{
                point.meta.value(QStringLiteral("legendLabel")).toString().trimmed().isEmpty()
                    ? point.label
                    : point.meta.value(QStringLiteral("legendLabel")).toString().trimmed(),
                point.meta.value(QStringLiteral("legendTooltip")).toString().trimmed().isEmpty()
                    ? point.tooltip
                    : point.meta.value(QStringLiteral("legendTooltip")).toString().trimmed(),
                point.color.isValid() ? point.color : primary.color,
                0,
                pointIndex});
        }
        for (int i = 1; i < dataSet.series.size(); ++i) {
            const ChartSeries& series = dataSet.series.at(i);
            pending.append(PendingLegendEntry{
                series.meta.value(QStringLiteral("legendLabel")).toString().trimmed().isEmpty()
                    ? series.name
                    : series.meta.value(QStringLiteral("legendLabel")).toString().trimmed(),
                series.meta.value(QStringLiteral("legendTooltip")).toString().trimmed().isEmpty()
                    ? series.name
                    : series.meta.value(QStringLiteral("legendTooltip")).toString().trimmed(),
                series.color.isValid() ? series.color : QColor(QStringLiteral("#333333")),
                i,
                -1});
        }
    } else {
        pending.reserve(dataSet.series.size());
        for (int i = 0; i < dataSet.series.size(); ++i) {
            const ChartSeries& series = dataSet.series.at(i);
            pending.append(PendingLegendEntry{
                series.meta.value(QStringLiteral("legendLabel")).toString().trimmed().isEmpty()
                    ? series.name
                    : series.meta.value(QStringLiteral("legendLabel")).toString().trimmed(),
                series.meta.value(QStringLiteral("legendTooltip")).toString().trimmed().isEmpty()
                    ? series.name
                    : series.meta.value(QStringLiteral("legendTooltip")).toString().trimmed(),
                series.color.isValid() ? series.color : QColor(QStringLiteral("#333333")),
                i,
                -1});
        }
    }

    if (pending.isEmpty()) {
        return;
    }

    const double legendWidth = legend_band_width(dataSet);
    const QRectF legendRect(scene.plotRect.right() + secondary_axis_band_width(dataSet) +
                                ((dataSet.kind == ChartKind::Pareto) ? 2.0 : 18.0),
                            scene.plotRect.top() + 12.0, qMax(96.0, legendWidth - 26.0),
                            scene.plotRect.height() + 48.0);
    const int entryCount = pending.size();
    const bool compactLegend = legendFromPoints && entryCount > 12;
    const qreal minRowHeight = compactLegend ? 10.0 : 20.0;
    const int columns = compactLegend
                            ? qMin(2, qMax(1, static_cast<int>(std::ceil((entryCount * minRowHeight) / legendRect.height()))))
                            : 1;
    const qreal columnGap = (columns > 1) ? 8.0 : 0.0;
    const qreal columnWidth = (legendRect.width() - ((columns - 1) * columnGap)) / columns;
    const int rowsPerColumn = qMax(1, static_cast<int>(std::ceil(static_cast<double>(entryCount) / columns)));
    const qreal rowHeight = qBound<qreal>(minRowHeight, legendRect.height() / rowsPerColumn, compactLegend ? 18.0 : 22.0);
    const qreal fontSize = qBound<qreal>(6.8, rowHeight - (columns > 1 ? 4.8 : 5.4), 9.5);
    QFont legendFont;
    legendFont.setPointSizeF(fontSize);
    const QFontMetricsF legendMetrics(legendFont);
    const qreal swatchSize = qBound<qreal>(6.0, rowHeight - 6.0, 12.0);
    const int maxEntries = rowsPerColumn * columns;

    for (int index = 0; index < qMin(entryCount, maxEntries); ++index) {
        const PendingLegendEntry& item = pending.at(index);
        if (item.label.trimmed().isEmpty()) {
            continue;
        }

        const int columnIndex = index / rowsPerColumn;
        const int rowIndex = index % rowsPerColumn;
        const QRectF row(legendRect.left() + (columnIndex * (columnWidth + columnGap)),
                         legendRect.top() + (rowIndex * rowHeight), columnWidth, rowHeight);
        const qreal textWidth = qMax<qreal>(18.0, row.width() - swatchSize - 10.0);
        const QString visibleLabel = legendMetrics.elidedText(item.label, Qt::ElideRight, static_cast<int>(std::floor(textWidth)));

        LegendEntry entry;
        entry.rect = row;
        entry.label = visibleLabel;
        entry.seriesIndex = item.seriesIndex;
        entry.elementIndex = item.elementIndex;
        entry.fontSize = fontSize;
        entry.color = (item.elementIndex >= 0) ? element_color(item.color, state, item.seriesIndex, item.elementIndex)
                                               : series_color(item.color, state, item.seriesIndex);
        scene.legends.append(entry);

        PickItem pick;
        pick.role = PickRole::Legend;
        pick.seriesIndex = item.seriesIndex;
        pick.elementIndex = item.elementIndex;
        pick.bounds = row.adjusted(-3.0, -2.0, 3.0, 2.0);
        pick.tooltip = item.tooltip.trimmed().isEmpty() ? item.label : item.tooltip;
        pick.path.addRect(pick.bounds);
        scene.picks.append(pick);
    }
}

void add_plot_background(ChartScene& scene, const ChartDataSet& dataSet) {
    append_rect(scene.fillVertices, QRectF(0.0, 0.0,
                                           scene.plotRect.right() + secondary_axis_band_width(dataSet) +
                                               legend_band_width(dataSet) + 40.0,
                                           scene.plotRect.bottom() + 60.0),
                scene.background);
    append_rect(scene.fillVertices, scene.plotRect, scene.plotBackground);
    append_rect_outline(scene.lineVertices, scene.plotRect, QColor(QStringLiteral("#d1d5db")));
}

bool grid_overlay_enabled(const ChartDataSet& dataSet) {
    return dataSet.meta.value(QStringLiteral("gridOverlay")).toBool();
}

void append_grid_vertical(ChartScene& scene, const ChartDataSet& dataSet, double x, const QColor& color,
                          double thickness = 1.0) {
    if (grid_overlay_enabled(dataSet)) {
        append_line(scene.lineVertices, QPointF(x, scene.plotRect.top()), QPointF(x, scene.plotRect.bottom()), color);
        return;
    }
    append_vertical_strip(scene.fillVertices, scene.plotRect, x, color, thickness);
}

void append_grid_horizontal(ChartScene& scene, const ChartDataSet& dataSet, double y, const QColor& color,
                            double thickness = 1.0) {
    if (grid_overlay_enabled(dataSet)) {
        append_line(scene.lineVertices, QPointF(scene.plotRect.left(), y), QPointF(scene.plotRect.right(), y), color);
        return;
    }
    append_horizontal_strip(scene.fillVertices, scene.plotRect, y, color, thickness);
}

struct AxisWindow {
    double xMin = 0.0;
    double xMax = 1.0;
    double yMin = 0.0;
    double yMax = 1.0;
    bool xIsDateTime = false;
    bool yLog = false;
    QString xSuffix;
    QStringList yCategories;
};

double map_linear(double value, double minValue, double maxValue, double start, double end) {
    if (!std::isfinite(value) || !std::isfinite(minValue) || !std::isfinite(maxValue)) {
        return start;
    }
    if (std::abs(maxValue - minValue) <= std::numeric_limits<double>::epsilon()) {
        return start;
    }
    const double t = (value - minValue) / (maxValue - minValue);
    return start + (t * (end - start));
}

double map_y_value(double value, const AxisWindow& axis, const QRectF& plotRect) {
    if (axis.yLog) {
        const double safeMin = qMax(kMinLogAxisValue, axis.yMin);
        const double safeMax = qMax(safeMin * 10.0, axis.yMax);
        const double safeValue = qBound(safeMin, value, safeMax);
        return map_linear(std::log10(safeValue), std::log10(safeMin), std::log10(safeMax), plotRect.bottom(),
                          plotRect.top());
    }
    return map_linear(value, axis.yMin, axis.yMax, plotRect.bottom(), plotRect.top());
}

AxisWindow cartesian_window(const ChartDataSet& dataSet, const ChartViewState& state, bool secondary = false) {
    AxisWindow axis;
    axis.xIsDateTime = dataSet.meta.value(QStringLiteral("xIsDateTime")).toBool();
    axis.yLog = dataSet.meta.value(QStringLiteral("yLogScale")).toBool();
    axis.xSuffix = dataSet.meta.value(QStringLiteral("xTickSuffix")).toString();

    if (!dataSet.meta.value(QStringLiteral("yCategories")).toList().isEmpty()) {
        const QVariantList raw = dataSet.meta.value(QStringLiteral("yCategories")).toList();
        for (const QVariant& item : raw) {
            axis.yCategories.append(item.toString());
        }
    }

    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();
    double primaryTotalY = 0.0;

    if (dataSet.kind == ChartKind::Bars || dataSet.kind == ChartKind::Histogram || dataSet.kind == ChartKind::Pareto ||
        dataSet.kind == ChartKind::ComboBarsLine || dataSet.kind == ChartKind::DotPlot) {
        minX = -0.5;
        maxX = qMax(0.5, static_cast<double>(qMax(1, dataSet.categories.size())) - 0.5);
    }

    if (dataSet.kind == ChartKind::BoxPlot) {
        minX = -0.5;
        maxX = qMax(0.5, static_cast<double>(qMax(1, dataSet.boxes.size())) - 0.5);
        minY = std::numeric_limits<double>::max();
        maxY = std::numeric_limits<double>::lowest();
        for (const BoxEntry& box : dataSet.boxes) {
            minY = qMin(minY, box.minimum);
            maxY = qMax(maxY, box.maximum);
            for (double outlier : box.outliers) {
                minY = qMin(minY, outlier);
                maxY = qMax(maxY, outlier);
            }
        }
    } else {
        for (int seriesIndex = 0; seriesIndex < dataSet.series.size(); ++seriesIndex) {
            const ChartSeries& series = dataSet.series.at(seriesIndex);
            const bool lineSecondary = secondary && series.role == QStringLiteral("line_secondary");
            const bool primaryAllowed = !secondary && series.role != QStringLiteral("line_secondary");
            if (!(lineSecondary || primaryAllowed)) {
                continue;
            }
            for (const ChartPoint& point : series.points) {
                minX = qMin(minX, point.x);
                maxX = qMax(maxX, point.x);
                minY = qMin(minY, point.y);
                maxY = qMax(maxY, point.y);
                if (!secondary && dataSet.kind == ChartKind::Pareto && series.role != QStringLiteral("line_secondary")) {
                    primaryTotalY += qMax(0.0, point.y);
                }
            }
        }
    }

    if (!std::isfinite(minX) || !std::isfinite(maxX) || minX >= maxX) {
        minX = 0.0;
        maxX = qMax(1.0, static_cast<double>(qMax(1, dataSet.categories.size())));
    }
    if (!std::isfinite(minY) || !std::isfinite(maxY) || minY >= maxY) {
        minY = axis.yLog ? 1.0e-5 : 0.0;
        maxY = axis.yLog ? 100.0 : 1.0;
    }

    if (dataSet.kind == ChartKind::Bars || dataSet.kind == ChartKind::Histogram || dataSet.kind == ChartKind::Pareto ||
        dataSet.kind == ChartKind::ComboBarsLine || dataSet.kind == ChartKind::Timeline ||
        dataSet.kind == ChartKind::TemporalScatter || dataSet.kind == ChartKind::Scatter ||
        dataSet.kind == ChartKind::DotPlot) {
        if (axis.yCategories.isEmpty()) {
            minY = axis.yLog ? qMax(kMinLogAxisValue, minY) : 0.0;
        }
        if (!secondary && dataSet.kind != ChartKind::Bars) {
            const QVariant forcedMax = dataSet.meta.value(QStringLiteral("yMax"));
            if (forcedMax.isValid()) {
                maxY = qMax(maxY, forcedMax.toDouble());
            }
        }
        if (secondary && dataSet.kind == ChartKind::Pareto) {
            minY = 0.0;
            maxY = 100.0;
        } else if (!secondary && dataSet.kind == ChartKind::Pareto && primaryTotalY > 0.0) {
            maxY = qMax(maxY, primaryTotalY);
        }
    }

    if (secondary && dataSet.kind == ChartKind::ComboBarsLine) {
        minY = 0.0;
        const QVariant forcedSecondaryMax = dataSet.meta.value(QStringLiteral("y2Max"));
        if (forcedSecondaryMax.isValid() && forcedSecondaryMax.toDouble() > 0.0) {
            maxY = rounded_count_axis_max(qMax(maxY, forcedSecondaryMax.toDouble()));
        } else {
            maxY = qMax(1.0, maxY * 1.05);
        }
    } else if (!secondary) {
        if (dataSet.kind == ChartKind::BoxPlot) {
            const auto paddedRange =
                (!axis.yLog && boxplot_uses_compact_hour_layout(dataSet))
                    ? padded_count_boxplot_y_range(minY, maxY)
                    : padded_boxplot_y_range(minY, maxY, axis.yLog);
            minY = paddedRange.first;
            maxY = paddedRange.second;
        } else if ((dataSet.kind == ChartKind::Bars || dataSet.kind == ChartKind::Histogram ||
             dataSet.kind == ChartKind::Pareto || dataSet.kind == ChartKind::ComboBarsLine) &&
            !axis.yLog) {
            maxY = rounded_count_axis_max(maxY);
        } else {
            maxY = axis.yLog ? qMax(maxY, minY * 10.0) : nice_ceil(maxY * 1.06);
        }
    }

    if (dataSet.meta.value(QStringLiteral("xMin")).isValid() && dataSet.meta.value(QStringLiteral("xMax")).isValid()) {
        const double forcedMin = dataSet.meta.value(QStringLiteral("xMin")).toDouble();
        const double forcedMax = dataSet.meta.value(QStringLiteral("xMax")).toDouble();
        if (forcedMin < forcedMax) {
            minX = forcedMin;
            maxX = forcedMax;
        }
    }
    if (dataSet.meta.value(QStringLiteral("yMin")).isValid() && dataSet.meta.value(QStringLiteral("yMax")).isValid() &&
        !secondary) {
        const double forcedMin = dataSet.meta.value(QStringLiteral("yMin")).toDouble();
        const double forcedMax = dataSet.meta.value(QStringLiteral("yMax")).toDouble();
        if (forcedMin < forcedMax) {
            minY = forcedMin;
            maxY = forcedMax;
        }
    }

    if (state.hasDataWindow) {
        minX = state.dataWindow.left();
        maxX = state.dataWindow.right();
    }

    if (!axis.yCategories.isEmpty() && !secondary) {
        minY = 0.5;
        maxY = static_cast<double>(axis.yCategories.size()) + 0.5;
    }

    axis.xMin = minX;
    axis.xMax = maxX;
    axis.yMin = minY;
    axis.yMax = maxY;
    return axis;
}

void add_pick(ChartScene& scene, PickRole role, int seriesIndex, int elementIndex, const QPainterPath& path,
              const QRectF& bounds, const QString& tooltip) {
    PickItem pick;
    pick.role = role;
    pick.seriesIndex = seriesIndex;
    pick.elementIndex = elementIndex;
    pick.path = path;
    pick.bounds = bounds;
    pick.tooltip = tooltip;
    scene.picks.append(pick);
}

void build_numeric_axes(ChartScene& scene, const ChartDataSet& dataSet, const AxisWindow& primary,
                        const AxisWindow* secondary = nullptr) {
    const QColor gridColor = grid_overlay_enabled(dataSet)
                                 ? alpha_color(QColor(QStringLiteral("#d1d9e6")), 180)
                                 : QColor(QStringLiteral("#eef1f5"));
    const bool categoricalX = (dataSet.kind == ChartKind::Bars || dataSet.kind == ChartKind::Histogram ||
                               dataSet.kind == ChartKind::Pareto || dataSet.kind == ChartKind::ComboBarsLine ||
                               dataSet.kind == ChartKind::DotPlot || dataSet.kind == ChartKind::BoxPlot);
    const int totalCategories = (dataSet.kind == ChartKind::BoxPlot) ? qMax(1, dataSet.boxes.size())
                                                                     : qMax(1, dataSet.categories.size());
    const bool forceAllBarLabels = (dataSet.kind == ChartKind::Bars);
    const bool forceAllHistogramLabels = (dataSet.kind == ChartKind::Histogram);
    const bool forceAllParetoLabels = (dataSet.kind == ChartKind::Pareto);
    const bool forceAllComboLabels = (dataSet.kind == ChartKind::ComboBarsLine);
    const bool dotPlotLabels = (dataSet.kind == ChartKind::DotPlot);
    const bool forceAllCategoryLabels =
        forceAllBarLabels || forceAllHistogramLabels || forceAllParetoLabels || forceAllComboLabels;
    const int xTicks = categoricalX
                           ? (forceAllCategoryLabels ? totalCategories : qMin(12, totalCategories))
                           : 6;

    if (categoricalX) {
        const int labelStep = forceAllCategoryLabels ? 1 : qMax(1, totalCategories / qMax(1, xTicks));
        const double categoryWidth = scene.plotRect.width() / static_cast<double>(totalCategories);
        const double labelBand =
            forceAllBarLabels ? qBound(72.0, rotated_label_band_height(dataSet.categories, 9.0, 45.0) + 14.0, 144.0)
                              : ((forceAllHistogramLabels || forceAllParetoLabels)
                                     ? 28.0
                                     : (forceAllComboLabels
                                            ? combo_bars_line_label_band_height(dataSet)
                                            : (dotPlotLabels ? dotplot_label_band_height(dataSet) : 0.0)));
        QFont categoryFont;
        categoryFont.setPointSizeF(9.0);
        const QFontMetricsF categoryMetrics(categoryFont);
        for (int i = 0; i < totalCategories; i += labelStep) {
            const double x = map_linear(static_cast<double>(i), primary.xMin, primary.xMax, scene.plotRect.left(),
                                        scene.plotRect.right());
            append_grid_vertical(scene, dataSet, x, gridColor, 1.0);
            if (dataSet.kind != ChartKind::BoxPlot) {
                const QString label = dataSet.categories.value(i);
                if (forceAllBarLabels) {
                    const double labelWidth =
                        qMax(categoryWidth * 1.4, static_cast<double>(categoryMetrics.horizontalAdvance(label)) + 10.0);
                    const double labelHeight = categoryMetrics.height() + 4.0;
                    const double labelCenterY = scene.plotRect.bottom() + 8.0 + (labelBand * 0.5);
                    scene.labels.append(OverlayLabel{
                        QRectF(x - (labelWidth * 0.5), labelCenterY - (labelHeight * 0.5), labelWidth, labelHeight),
                        label,
                        QColor(QStringLiteral("#374151")),
                        9.0,
                        Qt::AlignHCenter | Qt::AlignVCenter,
                        -45.0,
                        false});
                } else if (forceAllHistogramLabels || forceAllParetoLabels) {
                    const double labelWidth =
                        qMax(categoryWidth, static_cast<double>(categoryMetrics.horizontalAdvance(label)) + 6.0);
                    const double labelHeight = categoryMetrics.height() + 4.0;
                    const double labelCenterY = scene.plotRect.bottom() + 8.0 + (labelBand * 0.5);
                    scene.labels.append(OverlayLabel{
                        QRectF(x - (labelWidth * 0.5), labelCenterY - (labelHeight * 0.5), labelWidth, labelHeight),
                        label,
                        QColor(QStringLiteral("#374151")),
                        9.0,
                        Qt::AlignHCenter | Qt::AlignVCenter,
                        0.0,
                        false});
                } else if (forceAllComboLabels) {
                    const bool rotate = combo_bars_line_uses_rotated_dates(dataSet);
                    const double labelWidth = rotate
                                                  ? qMax(categoryWidth * 1.25,
                                                         static_cast<double>(categoryMetrics.horizontalAdvance(label)) +
                                                             8.0)
                                                  : qMax(categoryWidth,
                                                         static_cast<double>(categoryMetrics.horizontalAdvance(label)) +
                                                             8.0);
                    const double labelHeight = categoryMetrics.height() + 4.0;
                    const double labelCenterY = scene.plotRect.bottom() + 8.0 + (labelBand * 0.5);
                    scene.labels.append(OverlayLabel{
                        QRectF(x - (labelWidth * 0.5), labelCenterY - (labelHeight * 0.5), labelWidth, labelHeight),
                        label,
                        QColor(QStringLiteral("#374151")),
                        9.0,
                        Qt::AlignHCenter | Qt::AlignVCenter,
                        rotate ? -45.0 : 0.0,
                        false});
                } else if (dotPlotLabels) {
                    const bool rotate = dotplot_uses_rotated_labels(dataSet);
                    const double labelWidth = rotate
                                                  ? qMax(categoryWidth * labelStep * 1.45,
                                                         static_cast<double>(categoryMetrics.horizontalAdvance(label)) +
                                                             10.0)
                                                  : qMax(qBound(44.0, categoryWidth * labelStep * 1.35, 128.0),
                                                         static_cast<double>(categoryMetrics.horizontalAdvance(label)) +
                                                             8.0);
                    const double labelHeight = categoryMetrics.height() + 4.0;
                    const double labelCenterY = scene.plotRect.bottom() + 8.0 + (labelBand * 0.5);
                    scene.labels.append(OverlayLabel{
                        QRectF(x - (labelWidth * 0.5), labelCenterY - (labelHeight * 0.5), labelWidth, labelHeight),
                        label,
                        QColor(QStringLiteral("#374151")),
                        9.0,
                        Qt::AlignHCenter | Qt::AlignVCenter,
                        rotate ? -42.0 : 0.0,
                        false});
                } else {
                    const double labelWidth = qBound(44.0, categoryWidth * labelStep * 1.35, 128.0);
                    const bool rotate = label.size() > 9 || labelWidth < 68.0;
                    scene.labels.append(OverlayLabel{
                        QRectF(x - (labelWidth * 0.5), scene.plotRect.bottom() + 8.0, labelWidth, rotate ? 34.0 : 24.0),
                        label,
                        QColor(QStringLiteral("#374151")),
                        9.0,
                        Qt::AlignHCenter | Qt::AlignTop,
                        rotate ? -42.0 : 0.0,
                        false});
                }
            }
        }
    } else {
        for (int i = 0; i < xTicks; ++i) {
            const double ratio = (xTicks == 1) ? 0.0 : static_cast<double>(i) / static_cast<double>(xTicks - 1);
            const double xValue = primary.xMin + ((primary.xMax - primary.xMin) * ratio);
            const double x = map_linear(xValue, primary.xMin, primary.xMax, scene.plotRect.left(), scene.plotRect.right());
            append_grid_vertical(scene, dataSet, x, gridColor, 1.0);

            QString label;
            if (primary.xIsDateTime) {
                const qint64 span = static_cast<qint64>(primary.xMax - primary.xMin);
                const QLocale locale;
                const QDateTime dt = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(std::llround(xValue)));
                label = span >= (2LL * 24 * 60 * 60 * 1000) ? locale.toString(dt, QLocale::ShortFormat)
                                                            : locale.toString(dt.time(), QLocale::ShortFormat);
            } else {
                label = QString::number(xValue, 'f', std::abs(xValue) >= 10.0 ? 0 : 2) + primary.xSuffix;
            }

            scene.labels.append(OverlayLabel{
                QRectF(x - 42.0, scene.plotRect.bottom() + 8.0, 84.0, 20.0),
                label,
                QColor(QStringLiteral("#374151")),
                9.0,
                Qt::AlignHCenter | Qt::AlignTop,
                0.0,
                false});
        }
    }

    if (!primary.yCategories.isEmpty()) {
        const double yTitleBand =
            dataSet.yLabel.trimmed().isEmpty() ? 0.0 : (y_axis_title_band_width(dataSet.yLabel, 10.0) + 8.0);
        const double categoryLeft = 8.0 + yTitleBand;
        const double categoryWidth = qMax(62.0, scene.plotRect.left() - categoryLeft - 10.0);
        const int count = primary.yCategories.size();
        for (int i = 0; i < count; ++i) {
            const double yValue = static_cast<double>(i + 1);
            const double y = map_linear(yValue, primary.yMin, primary.yMax, scene.plotRect.bottom(), scene.plotRect.top());
            append_grid_horizontal(scene, dataSet, y, gridColor, 1.0);
            scene.labels.append(OverlayLabel{
                QRectF(categoryLeft, y - 12.0, categoryWidth, 24.0),
                primary.yCategories.at(i),
                QColor(QStringLiteral("#374151")),
                9.0,
                Qt::AlignRight | Qt::AlignVCenter,
                0.0,
                false});
        }
    } else {
        const int yTicks = 6;
        for (int i = 0; i < yTicks; ++i) {
            const double ratio = static_cast<double>(i) / static_cast<double>(yTicks - 1);
            double value = 0.0;
            if (primary.yLog) {
                const double lo = std::log10(qMax(kMinLogAxisValue, primary.yMin));
                const double hi = std::log10(qMax(primary.yMin * 10.0, primary.yMax));
                value = std::pow(10.0, lo + ((hi - lo) * ratio));
            } else {
                value = primary.yMin + ((primary.yMax - primary.yMin) * ratio);
            }
            const double y = map_y_value(value, primary, scene.plotRect);
            append_grid_horizontal(scene, dataSet, y, gridColor, 1.0);
            scene.labels.append(OverlayLabel{
                QRectF(10.0, y - 10.0, qMax(56.0, scene.plotRect.left() - 20.0), 20.0),
                primary.yLog ? QString::number(value, 'e', 1) : QString::number(value, 'f', value >= 10.0 ? 0 : 2),
                QColor(QStringLiteral("#374151")),
                9.0,
                Qt::AlignRight | Qt::AlignVCenter,
                0.0,
                false});

            if (secondary != nullptr) {
                const double secondaryValue = secondary->yMin + ((secondary->yMax - secondary->yMin) * ratio);
                QString secondaryText = QLocale().toString(secondaryValue, 'f', dataSet.kind == ChartKind::Pareto ? 2 : (secondaryValue >= 10.0 ? 0 : 1));
                if (dataSet.kind == ChartKind::Pareto) {
                    secondaryText += QStringLiteral("%");
                }
                scene.labels.append(OverlayLabel{
                    QRectF(scene.plotRect.right() + 8.0, y - 10.0, (dataSet.kind == ChartKind::Pareto) ? 58.0 : 52.0, 20.0),
                    secondaryText,
                    QColor(QStringLiteral("#374151")),
                    9.0,
                    Qt::AlignLeft | Qt::AlignVCenter,
                    0.0,
                    false});
            }
        }
    }

    add_axis_titles(scene, dataSet);
}

void build_pie(ChartScene& scene, const ChartDataSet& dataSet, const QSizeF& size, const ChartViewState& state) {
    add_title_block(scene, dataSet, size);
    scene.plotRect = QRectF(kLeftMargin, kTopMargin + 10.0, size.width() - kLeftMargin - kRightMargin - kLegendWidth,
                            size.height() - kTopMargin - kBottomMargin);
    add_plot_background(scene, dataSet);

    if (dataSet.series.isEmpty() || dataSet.series.constFirst().points.isEmpty()) {
        scene.labels.append(OverlayLabel{scene.plotRect, QStringLiteral("Sem dados para exibir."),
                                         QColor(QStringLiteral("#4b5563")), 12.0,
                                         Qt::AlignCenter, 0.0, false});
        return;
    }

    const ChartSeries& series = dataSet.series.constFirst();
    double total = 0.0;
    for (const ChartPoint& point : series.points) {
        total += qMax(0.0, point.y);
    }

    const QPointF center(scene.plotRect.center().x() - 10.0, scene.plotRect.center().y() + 6.0);
    const double outerRadius = qMin(scene.plotRect.width(), scene.plotRect.height()) * 0.34;
    const double innerRadius = outerRadius * 0.45;
    const qreal connectorLaneGap = 24.0;
    const qreal labelColumnGap = 12.0;
    const qreal connectorLeadDistance = 16.0;
    const qreal leftLaneX = center.x() - outerRadius - connectorLaneGap;
    const qreal rightLaneX = center.x() + outerRadius + connectorLaneGap;
    const qreal leftAnchorX = leftLaneX - labelColumnGap;
    const qreal rightAnchorX = rightLaneX + labelColumnGap;
    const qreal activeConnectorShift = 12.0;
    double startAngle = 90.0;

    struct ExternalPieLabel {
        int elementIndex = -1;
        QString text;
        QString tooltip;
        QPointF edgePoint;
        QPointF radialPoint;
        QPointF lanePoint;
        QPointF anchorPoint;
        bool leftSide = false;
        bool active = false;
        QColor color;
        qreal fontSize = 10.0;
        qreal width = 0.0;
        qreal height = 0.0;
    };
    QVector<ExternalPieLabel> externalLabels;

    for (int i = 0; i < series.points.size(); ++i) {
        const ChartPoint& point = series.points.at(i);
        const double sweepAngle = total > 0.0 ? (point.y / total) * 360.0 * state.animationT : 0.0;
        const int activeSeriesIndex = 0;
        const bool active = active_element(state, activeSeriesIndex, i);
        const QColor fill = element_color(point.color.isValid() ? point.color : QColor(QStringLiteral("#0057ff")),
                                          state, activeSeriesIndex, i);
        const QColor stroke = fill.darker(active ? 150 : 125);
        const double explode = active ? 12.0 : 0.0;
        const double midAngle = qDegreesToRadians(startAngle + (sweepAngle * 0.5));
        const QPointF offset(std::cos(midAngle) * explode, -std::sin(midAngle) * explode);
        append_donut_slice(scene.fillVertices, scene.lineVertices, center + offset, innerRadius,
                           outerRadius, startAngle, sweepAngle, fill, stroke);

        QPainterPath path;
        path.arcMoveTo(QRectF((center + offset).x() - outerRadius, (center + offset).y() - outerRadius,
                              outerRadius * 2.0, outerRadius * 2.0), startAngle);
        path.arcTo(QRectF((center + offset).x() - outerRadius, (center + offset).y() - outerRadius,
                          outerRadius * 2.0, outerRadius * 2.0), startAngle, sweepAngle);
        path.arcTo(QRectF((center + offset).x() - innerRadius, (center + offset).y() - innerRadius,
                          innerRadius * 2.0, innerRadius * 2.0), startAngle + sweepAngle, -sweepAngle);
        path.closeSubpath();
        add_pick(scene, PickRole::Data, 0, i, path, path.boundingRect(), point.tooltip);

        const double percent = total > 0.0 ? (point.y / total) * 100.0 : 0.0;
        const QString percentText = QStringLiteral("%1%").arg(QLocale().toString(percent, 'f', 2));
        const qreal fontSize = active ? 11.0 : 10.0;
        QFont labelFont;
        labelFont.setPointSizeF(fontSize);
        labelFont.setBold(true);
        const QFontMetricsF metrics(labelFont);
        const qreal textWidth = metrics.horizontalAdvance(percentText);
        const qreal textHeight = metrics.height();
        const bool leftSide = (std::cos(midAngle) < 0.0);
        const qreal sideShift = active ? (leftSide ? -activeConnectorShift : activeConnectorShift) : 0.0;
        const QPointF edgePoint(center.x() + offset.x() + (std::cos(midAngle) * (outerRadius + 2.0)),
                                center.y() + offset.y() - (std::sin(midAngle) * (outerRadius + 2.0)));
        const QPointF radialPoint(center.x() + offset.x() + (std::cos(midAngle) * (outerRadius + connectorLeadDistance)),
                                  center.y() + offset.y() - (std::sin(midAngle) * (outerRadius + connectorLeadDistance)));
        const qreal laneX = (leftSide ? leftLaneX : rightLaneX) + sideShift;
        const QPointF lanePoint(laneX, radialPoint.y());
        const QPointF anchorPoint((leftSide ? leftAnchorX : rightAnchorX) + sideShift, radialPoint.y());
        externalLabels.append(ExternalPieLabel{i,
                                               percentText,
                                               point.tooltip,
                                               edgePoint,
                                               radialPoint,
                                               lanePoint,
                                               anchorPoint,
                                               leftSide,
                                               active,
                                               fill.darker(active ? 165 : 145),
                                               fontSize,
                                               textWidth,
                                               textHeight});

        startAngle += sweepAngle;
    }

    auto layoutExternalLabels = [&](bool leftSide) {
        QVector<int> indexes;
        for (int idx = 0; idx < externalLabels.size(); ++idx) {
            if (externalLabels.at(idx).leftSide == leftSide) {
                indexes.append(idx);
            }
        }
        std::sort(indexes.begin(), indexes.end(), [&](int a, int b) {
            return externalLabels.at(a).anchorPoint.y() < externalLabels.at(b).anchorPoint.y();
        });
        if (indexes.isEmpty()) {
            return;
        }

        const qreal topLimit = scene.plotRect.top() + 8.0;
        const qreal bottomLimit = scene.plotRect.bottom() - 8.0;
        qreal averageHeight = 0.0;
        for (int idx : indexes) {
            averageHeight += externalLabels.at(idx).height;
        }
        averageHeight /= static_cast<qreal>(indexes.size());
        const qreal availableHeight = bottomLimit - topLimit;
        const qreal minGap = indexes.size() > 1
                                 ? qBound<qreal>(6.0,
                                                 (availableHeight - (averageHeight * static_cast<qreal>(indexes.size()))) /
                                                     static_cast<qreal>(indexes.size() - 1),
                                                 14.0)
                                 : 14.0;
        qreal currentY = topLimit;
        for (int idx : indexes) {
            ExternalPieLabel& label = externalLabels[idx];
            const qreal halfHeight = (label.height * 0.5);
            qreal centerY = qMax(label.anchorPoint.y(), currentY + halfHeight);
            centerY = qMin(centerY, bottomLimit - halfHeight);
            label.anchorPoint.setY(centerY);
            currentY = centerY + halfHeight + minGap;
        }

        qreal overflow = currentY - bottomLimit;
        if (overflow > 0.0) {
            for (int pos = indexes.size() - 1; pos >= 0; --pos) {
                ExternalPieLabel& label = externalLabels[indexes.at(pos)];
                const qreal halfHeight = (label.height * 0.5);
                const qreal maxY = bottomLimit - halfHeight - overflow;
                const qreal adjustedY = qMin(label.anchorPoint.y(), maxY);
                label.anchorPoint.setY(adjustedY);
                overflow = qMax<qreal>(0.0, overflow - (halfHeight + minGap));
            }
        }

        for (int idx : indexes) {
            const ExternalPieLabel& label = externalLabels.at(idx);
            const QPointF elbowPoint(label.lanePoint.x(), label.anchorPoint.y());
            append_line(scene.lineVertices, label.edgePoint, label.radialPoint, label.color);
            append_line(scene.lineVertices, label.radialPoint, label.lanePoint, label.color);
            if (std::abs(elbowPoint.y() - label.lanePoint.y()) > 0.5) {
                append_line(scene.lineVertices, label.lanePoint, elbowPoint, label.color);
            }
            if (std::abs(label.anchorPoint.x() - elbowPoint.x()) > 0.5) {
                append_line(scene.lineVertices, elbowPoint, label.anchorPoint, label.color);
            }

            QPainterPath connectorPath;
            connectorPath.moveTo(label.edgePoint);
            connectorPath.lineTo(label.radialPoint);
            connectorPath.lineTo(label.lanePoint);
            if (std::abs(elbowPoint.y() - label.lanePoint.y()) > 0.5) {
                connectorPath.lineTo(elbowPoint);
            }
            if (std::abs(label.anchorPoint.x() - elbowPoint.x()) > 0.5) {
                connectorPath.lineTo(label.anchorPoint);
            }
            QPainterPathStroker stroker;
            stroker.setWidth(10.0);
            stroker.setCapStyle(Qt::RoundCap);
            stroker.setJoinStyle(Qt::RoundJoin);
            const QPainterPath connectorHit = stroker.createStroke(connectorPath);
            add_pick(scene, PickRole::Legend, 0, label.elementIndex, connectorHit,
                     connectorHit.boundingRect(), label.tooltip);

            const qreal labelGap = 6.0;
            const QRectF rect(
                leftSide ? (label.anchorPoint.x() - label.width - labelGap) : (label.anchorPoint.x() + labelGap),
                label.anchorPoint.y() - (label.height * 0.5), label.width + 4.0, label.height + 2.0);
            scene.labels.append(OverlayLabel{
                rect,
                label.text,
                label.color,
                label.fontSize,
                leftSide ? (Qt::AlignRight | Qt::AlignVCenter) : (Qt::AlignLeft | Qt::AlignVCenter),
                0.0,
                label.active});

            QPainterPath labelPath;
            labelPath.addRect(rect.adjusted(-3.0, -2.0, 3.0, 2.0));
            add_pick(scene, PickRole::Legend, 0, label.elementIndex, labelPath, labelPath.boundingRect(), label.tooltip);
        }
    };

    layoutExternalLabels(false);
    layoutExternalLabels(true);

    const QString totalValueText = QString::number(total, 'f', total >= 10.0 ? 0 : 2);
    const QString totalText = QStringLiteral("Total\n%1").arg(totalValueText);
    QFont totalFont;
    totalFont.setPointSizeF(11.0);
    totalFont.setBold(true);
    const QFontMetricsF totalMetrics(totalFont);
    const qreal totalTextWidth =
        qMax(totalMetrics.horizontalAdvance(QStringLiteral("Total")), totalMetrics.horizontalAdvance(totalValueText));
    const qreal totalTextHeight = (totalMetrics.height() * 2.0) + totalMetrics.leading();
    const qreal totalBoxWidth =
        qMin<qreal>(qMax<qreal>(totalTextWidth + 18.0, innerRadius * 1.4), (innerRadius * 2.0) - 8.0);
    const qreal totalBoxHeight =
        qMin<qreal>(qMax<qreal>(totalTextHeight + 10.0, innerRadius * 0.9), (innerRadius * 2.0) - 8.0);
    scene.labels.append(OverlayLabel{
        QRectF(center.x() - (totalBoxWidth * 0.5), center.y() - (totalBoxHeight * 0.5), totalBoxWidth, totalBoxHeight),
        totalText,
        QColor(QStringLiteral("#111827")),
        11.0,
        Qt::AlignCenter,
        0.0,
        true});

    add_legend(scene, dataSet, state);
}

void build_bars(ChartScene& scene, const ChartDataSet& dataSet, const QSizeF& size, const ChartViewState& state,
                bool secondaryLineVisible = true) {
    add_title_block(scene, dataSet, size);
    scene.plotRect = plot_rect_for(dataSet, size);
    add_plot_background(scene, dataSet);
    const AxisWindow primary = cartesian_window(dataSet, state, false);
    const bool hasSecondary = (dataSet.kind == ChartKind::Pareto || dataSet.kind == ChartKind::ComboBarsLine);
    const AxisWindow secondary = hasSecondary ? cartesian_window(dataSet, state, true) : AxisWindow{};
    build_numeric_axes(scene, dataSet, primary, hasSecondary ? &secondary : nullptr);

    const int totalSeries = dataSet.series.size();
    int barSeriesCount = 0;
    for (const ChartSeries& series : dataSet.series) {
        if (series.role == QStringLiteral("bar") || series.role.isEmpty()) {
            ++barSeriesCount;
        }
    }
    barSeriesCount = qMax(1, barSeriesCount);

    int currentBarSeries = 0;
    for (int seriesIndex = 0; seriesIndex < totalSeries; ++seriesIndex) {
        const ChartSeries& series = dataSet.series.at(seriesIndex);
        if (series.role == QStringLiteral("line_secondary") || series.role == QStringLiteral("mean")) {
            continue;
        }

        const double groupWidth = 0.82;
        const double slotWidth = groupWidth / static_cast<double>(barSeriesCount);
        const bool usePointColors = series.meta.value(QStringLiteral("usePointColors")).toBool();
        for (int pointIndex = 0; pointIndex < series.points.size(); ++pointIndex) {
            const ChartPoint& point = series.points.at(pointIndex);
            const double animatedValue = qMax(primary.yMin, point.y * state.animationT);
            const double baseX = point.x - (groupWidth * 0.5) + (currentBarSeries * slotWidth);
            const QRectF rect(
                map_linear(baseX, primary.xMin, primary.xMax, scene.plotRect.left(), scene.plotRect.right()),
                map_y_value(animatedValue, primary, scene.plotRect),
                map_linear(baseX + slotWidth, primary.xMin, primary.xMax, scene.plotRect.left(), scene.plotRect.right()) -
                    map_linear(baseX, primary.xMin, primary.xMax, scene.plotRect.left(), scene.plotRect.right()),
                scene.plotRect.bottom() - map_y_value(animatedValue, primary, scene.plotRect));
            const QColor baseColor =
                usePointColors ? (point.color.isValid() ? point.color : QColor(QStringLiteral("#0057ff")))
                               : (series.color.isValid() ? series.color : QColor(QStringLiteral("#0057ff")));
            const QColor fill = plotted_element_fill_color(baseColor, state, seriesIndex, pointIndex);
            append_rect(scene.fillVertices, rect, fill);
            append_rect_outline(scene.lineVertices, rect.adjusted(0.5, 0.5, -0.5, -0.5), fill.darker(135));

            QPainterPath path;
            path.addRect(rect);
            add_pick(scene, PickRole::Data, seriesIndex, pointIndex, path, rect, point.tooltip);
        }
        ++currentBarSeries;
    }

    for (int seriesIndex = 0; seriesIndex < totalSeries; ++seriesIndex) {
        const ChartSeries& series = dataSet.series.at(seriesIndex);
        if (series.role != QStringLiteral("line_secondary") && series.role != QStringLiteral("mean")) {
            continue;
        }
        if (!secondaryLineVisible && series.role == QStringLiteral("line_secondary")) {
            continue;
        }

        QVector<QPointF> polyline;
        polyline.reserve(series.points.size());
        for (const ChartPoint& point : series.points) {
            const AxisWindow& axis = (series.role == QStringLiteral("line_secondary")) ? secondary : primary;
            polyline.append(QPointF(
                map_linear(point.x, primary.xMin, primary.xMax, scene.plotRect.left(), scene.plotRect.right()),
                map_y_value(point.y * state.animationT, axis, scene.plotRect)));
        }

        const QColor stroke = line_color(series.color.isValid() ? series.color : QColor(QStringLiteral("#111827")),
                                         state, seriesIndex);
        const bool highlightSecondaryArea =
            dataSet.kind == ChartKind::Pareto && series.role == QStringLiteral("line_secondary") &&
            active_series(state) == seriesIndex;
        if (highlightSecondaryArea && polyline.size() >= 2) {
            QVector<QPointF> area;
            area.reserve(polyline.size() + 2);
            const double baselineY = map_y_value(secondary.yMin, secondary, scene.plotRect);
            area.append(QPointF(polyline.constFirst().x(), baselineY));
            for (const QPointF& point : polyline) {
                area.append(point);
            }
            area.append(QPointF(polyline.constLast().x(), baselineY));
            append_polygon_fan(scene.fillVertices, area, alpha_color(stroke, 52));
        }
        append_polygon_outline(scene.lineVertices, polyline, stroke, false);
        if (polyline.size() >= 2) {
            QPainterPath linePath;
            linePath.moveTo(polyline.constFirst());
            for (int i = 1; i < polyline.size(); ++i) {
                linePath.lineTo(polyline.at(i));
            }
            QPainterPathStroker stroker;
            stroker.setCapStyle(Qt::RoundCap);
            stroker.setJoinStyle(Qt::RoundJoin);
            stroker.setWidth(10.0);
            const QPainterPath hitPath = stroker.createStroke(linePath);
            add_pick(scene, PickRole::Data, seriesIndex, -1, hitPath, hitPath.boundingRect(), series.name);
        }

        for (int pointIndex = 0; pointIndex < polyline.size(); ++pointIndex) {
            const bool highlight = active_element(state, seriesIndex, pointIndex);
            const double radius = highlight ? 5.2 : 3.8;
            append_circle(scene.fillVertices, scene.lineVertices, polyline.at(pointIndex), radius, stroke, stroke.darker(140));
            QPainterPath path;
            path.addEllipse(polyline.at(pointIndex), radius + 2.0, radius + 2.0);
            add_pick(scene, PickRole::Data, seriesIndex, pointIndex, path, path.boundingRect(),
                     series.points.at(pointIndex).tooltip);
        }
    }

    add_legend(scene, dataSet, state);
}

void build_scatter(ChartScene& scene, const ChartDataSet& dataSet, const QSizeF& size, const ChartViewState& state) {
    add_title_block(scene, dataSet, size);
    scene.plotRect = plot_rect_for(dataSet, size);
    add_plot_background(scene, dataSet);
    const AxisWindow axis = cartesian_window(dataSet, state, false);
    build_numeric_axes(scene, dataSet, axis, nullptr);

    int totalPointCount = 0;
    for (const ChartSeries& series : dataSet.series) {
        if (series.role != QStringLiteral("mean")) {
            totalPointCount += series.points.size();
        }
    }
    const bool fastPointCloud = dataSet.meta.value(QStringLiteral("fastPointCloud")).toBool() || totalPointCount > 6000;
    const int pointSegments = fastPointCloud ? 10 : 16;
    const auto point_center = [&](const ChartPoint& point) {
        return QPointF(map_linear(point.x, axis.xMin, axis.xMax, scene.plotRect.left(), scene.plotRect.right()),
                       map_y_value(point.y * state.animationT, axis, scene.plotRect));
    };
    if (totalPointCount > 0) {
        scene.fillVertices.reserve(scene.fillVertices.size() + (totalPointCount * pointSegments * 3));
        scene.lineVertices.reserve(scene.lineVertices.size() + (totalPointCount * pointSegments * 2));
        scene.picks.reserve(scene.picks.size() + totalPointCount);
    }

    for (int seriesIndex = 0; seriesIndex < dataSet.series.size(); ++seriesIndex) {
        const ChartSeries& series = dataSet.series.at(seriesIndex);
        if (series.role == QStringLiteral("mean")) {
            const QColor stroke = line_color(series.color, state, seriesIndex);
            QVector<QPointF> line;
            for (const ChartPoint& point : series.points) {
                line.append(QPointF(map_linear(point.x, axis.xMin, axis.xMax, scene.plotRect.left(), scene.plotRect.right()),
                                    map_y_value(point.y * state.animationT, axis, scene.plotRect)));
            }
            append_polygon_outline(scene.lineVertices, line, stroke, false);
            continue;
        }

        for (int pointIndex = 0; pointIndex < series.points.size(); ++pointIndex) {
            const ChartPoint& point = series.points.at(pointIndex);
            const QPointF center = point_center(point);
            const bool highlight = active_element(state, seriesIndex, pointIndex);
            QColor fill = series_color(series.color.isValid() ? series.color : QColor(QStringLiteral("#0057ff")),
                                       state, seriesIndex, highlight ? pointIndex : -1);
            const bool dimmedByOtherSeries = (active_series(state) >= 0 && active_series(state) != seriesIndex);
            const double radius = highlight ? 8.2 : 5.0;
            const QColor pointFill = alpha_color(fill, dimmedByOtherSeries ? inactive_point_alpha(fill) : 190);
            const QColor stroke = fill.darker(highlight ? 150 : 130);
            if (highlight) {
                append_circle_direct(scene.fillVertices, scene.lineVertices, center, radius + 3.4,
                                     alpha_color(fill, 54), alpha_color(fill, 105), 14);
            }
            if (fastPointCloud) {
                append_circle_direct(scene.fillVertices, scene.lineVertices, center, radius, pointFill, stroke,
                                     pointSegments);
            } else {
                append_circle(scene.fillVertices, scene.lineVertices, center, radius, pointFill, stroke,
                              pointSegments);
            }
            if (fastPointCloud) {
                const QRectF bounds(center.x() - radius - 3.0, center.y() - radius - 3.0,
                                    (radius + 3.0) * 2.0, (radius + 3.0) * 2.0);
                add_pick(scene, PickRole::Data, seriesIndex, pointIndex, QPainterPath(), bounds, point.tooltip);
            } else {
                QPainterPath path;
                path.addEllipse(center, radius + 2.5, radius + 2.5);
                add_pick(scene, PickRole::Data, seriesIndex, pointIndex, path, path.boundingRect(), point.tooltip);
            }
        }
    }

    add_legend(scene, dataSet, state);
}

void build_radar(ChartScene& scene, const ChartDataSet& dataSet, const QSizeF& size, const ChartViewState& state) {
    add_title_block(scene, dataSet, size);
    scene.plotRect = QRectF(kLeftMargin, kTopMargin + 10.0, size.width() - kLeftMargin - kRightMargin - kLegendWidth,
                            size.height() - kTopMargin - kBottomMargin);
    add_plot_background(scene, dataSet);

    const QPointF center(scene.plotRect.center());
    const double radius = qMin(scene.plotRect.width(), scene.plotRect.height()) * 0.34;
    const int curveSubdivisions = 12;
    const bool useSmoothCurve = dataSet.meta.value(QStringLiteral("smoothCurve"), true).toBool();
    const bool maskGridUnderSeries = dataSet.meta.value(QStringLiteral("maskGridUnderSeries"), false).toBool();
    const bool radarPointsForeground = dataSet.meta.value(QStringLiteral("radarPointsForeground"), false).toBool();
    const int surfaceAlpha =
        qBound(0, dataSet.meta.value(QStringLiteral("radarSurfaceAlpha"), 90).toInt(), 255);
    int maxValue = 1;
    for (const ChartSeries& series : dataSet.series) {
        for (const ChartPoint& point : series.points) {
            maxValue = qMax(maxValue, static_cast<int>(std::ceil(point.y)));
        }
    }
    const double radialMax = nice_ceil(static_cast<double>(maxValue) * 1.08);
    const int spokeCount = qMax(3, dataSet.categories.size());
    const double startAngle = -kHalfPi;
    const double angleStep = (kPi * 2.0) / static_cast<double>(spokeCount);
    const QColor ringColor = QColor(QStringLiteral("#e5e7eb"));
    const QColor spokeColor = QColor(QStringLiteral("#d1d5db"));
    const bool radarGridOverlay = dataSet.meta.value(QStringLiteral("radarGridOverlay")).toBool();

    for (int ring = 1; ring <= 5; ++ring) {
        const double ringRadius = radius * (static_cast<double>(ring) / 5.0);
        if (radarGridOverlay) {
            append_circle_outline(scene.lineVertices, center, ringRadius, alpha_color(ringColor, 185), 96);
        } else {
            append_circle_band(scene.fillVertices, center, ringRadius, 1.4, ringColor, 96);
        }
        scene.labels.append(OverlayLabel{
            QRectF(center.x() + 8.0, center.y() - ringRadius - 10.0, 38.0, 18.0),
            QString::number((radialMax / 5.0) * ring, 'f', ((radialMax / 5.0) * ring) >= 10.0 ? 0 : 1),
            QColor(QStringLiteral("#6b7280")),
            8.0,
            Qt::AlignLeft | Qt::AlignVCenter,
            0.0,
            false});
    }

    for (int i = 0; i < spokeCount; ++i) {
        const double angle = startAngle + ((kPi * 2.0) * static_cast<double>(i) / static_cast<double>(spokeCount));
        const QPointF outer = center + QPointF(std::cos(angle) * radius, std::sin(angle) * radius);
        if (radarGridOverlay) {
            append_line(scene.lineVertices, center, outer, alpha_color(spokeColor, 190));
        } else {
            append_line_band(scene.fillVertices, center, outer, 1.3, spokeColor);
        }

        const QPointF labelPoint = center + QPointF(std::cos(angle) * (radius + 24.0), std::sin(angle) * (radius + 24.0));
        scene.labels.append(OverlayLabel{
            QRectF(labelPoint.x() - 60.0, labelPoint.y() - 10.0, 120.0, 20.0),
            dataSet.categories.value(i),
            QColor(QStringLiteral("#374151")),
            9.0,
            Qt::AlignCenter,
            0.0,
            false});
    }

    for (int seriesIndex = 0; seriesIndex < dataSet.series.size(); ++seriesIndex) {
        const ChartSeries& series = dataSet.series.at(seriesIndex);
        QVector<QPointF> polygon;
        QVector<double> radialValues;
        polygon.reserve(spokeCount);
        radialValues.reserve(spokeCount);
        const QColor fill = alpha_color(series_color(series.color, state, seriesIndex), surfaceAlpha);
        const QColor stroke = line_color(series.color, state, seriesIndex);

        for (int i = 0; i < qMin(spokeCount, series.points.size()); ++i) {
            const ChartPoint& point = series.points.at(i);
            const double ratio = radialMax > 0.0 ? qBound(0.0, (point.y * state.animationT) / radialMax, 1.0) : 0.0;
            const double angle = startAngle + ((kPi * 2.0) * static_cast<double>(i) / static_cast<double>(spokeCount));
            const double radialValue = radius * ratio;
            radialValues.append(radialValue);
            polygon.append(center + QPointF(std::cos(angle) * radialValue, std::sin(angle) * radialValue));
        }

        QVector<QPointF> smoothCurve;
        if (useSmoothCurve && radialValues.size() >= 3) {
            smoothCurve.reserve(radialValues.size() * curveSubdivisions);
            for (int i = 0; i < radialValues.size(); ++i) {
                const double r0 = radialValues.at((i - 1 + radialValues.size()) % radialValues.size());
                const double r1 = radialValues.at(i);
                const double r2 = radialValues.at((i + 1) % radialValues.size());
                const double r3 = radialValues.at((i + 2) % radialValues.size());
                for (int step = 0; step < curveSubdivisions; ++step) {
                    const double t = static_cast<double>(step) / static_cast<double>(curveSubdivisions);
                    const double angle = startAngle + (static_cast<double>(i) + t) * angleStep;
                    const double curveRadius = qBound(0.0, catmull_rom(r0, r1, r2, r3, t), radius);
                    smoothCurve.append(center + QPointF(std::cos(angle) * curveRadius, std::sin(angle) * curveRadius));
                }
            }
        } else {
            smoothCurve = polygon;
        }

        if (maskGridUnderSeries && !smoothCurve.isEmpty()) {
            QVector<QPointF> gridMask;
            gridMask.reserve(smoothCurve.size());
            for (const QPointF& point : smoothCurve) {
                const QPointF vector = point - center;
                const double length = std::hypot(vector.x(), vector.y());
                if (length <= 0.0) {
                    gridMask.append(point);
                    continue;
                }
                const double maskedRadius = qMin(radius, length + 8.0);
                const double scale = maskedRadius / length;
                gridMask.append(center + QPointF(vector.x() * scale, vector.y() * scale));
            }
            append_polygon_fan_from_center(scene.fillVertices, center, gridMask, scene.plotBackground);
        }

        append_polygon_fan_from_center(scene.fillVertices, center, smoothCurve, fill);
        append_polygon_outline(scene.lineVertices, smoothCurve, stroke);

        QVector<ColoredVertex>& pointFillVertices =
            radarPointsForeground ? scene.foregroundFillVertices : scene.fillVertices;
        QVector<ColoredVertex>& pointLineVertices =
            radarPointsForeground ? scene.foregroundLineVertices : scene.lineVertices;
        for (int pointIndex = 0; pointIndex < polygon.size(); ++pointIndex) {
            const bool highlight = active_element(state, seriesIndex, pointIndex);
            QColor pointFill = stroke;
            QColor pointStroke = stroke.darker(140);
            if (radarPointsForeground) {
                pointFill.setAlpha(255);
                pointStroke.setAlpha(255);
                if (highlight) {
                    append_circle(pointFillVertices, pointLineVertices, polygon.at(pointIndex), 8.0,
                                  alpha_color(stroke, 54), alpha_color(stroke, 105), 18);
                }
            }
            append_circle(pointFillVertices, pointLineVertices, polygon.at(pointIndex),
                          highlight ? 5.6 : 4.0, pointFill, pointStroke, 14);
            QPainterPath path;
            path.addEllipse(polygon.at(pointIndex), 7.0, 7.0);
            add_pick(scene, PickRole::Data, seriesIndex, pointIndex, path, path.boundingRect(),
                     series.points.at(pointIndex).tooltip);
        }
    }

    add_legend(scene, dataSet, state);
}

void build_boxplot(ChartScene& scene, const ChartDataSet& dataSet, const QSizeF& size, const ChartViewState& state) {
    add_title_block(scene, dataSet, size);
    scene.plotRect = plot_rect_for(dataSet, size);
    add_plot_background(scene, dataSet);
    const AxisWindow axis = cartesian_window(dataSet, state, false);
    build_numeric_axes(scene, dataSet, axis, nullptr);
    const bool compactHourLayout = boxplot_uses_compact_hour_layout(dataSet);
    const bool rotateLabels = boxplot_uses_rotated_labels(dataSet);
    const double labelBand = boxplot_label_band_height(dataSet);
    QFont categoryFont;
    categoryFont.setPointSizeF(9.0);
    const QFontMetricsF categoryMetrics(categoryFont);

    const double slotWidth =
        map_linear(0.34, axis.xMin, axis.xMax, scene.plotRect.left(), scene.plotRect.right()) -
        map_linear(-0.34, axis.xMin, axis.xMax, scene.plotRect.left(), scene.plotRect.right());
    const double compactBoxWidth =
        qBound(4.0, (scene.plotRect.width() / static_cast<double>(qMax(1, dataSet.boxes.size()))) * 0.32, 12.0);
    const double bodyWidth = compactHourLayout ? compactBoxWidth : slotWidth;
    const double boxHalfWidth = bodyWidth * 0.5;
    const double whiskerHalfWidth = compactHourLayout ? qMax(2.0, bodyWidth * 0.34) : (slotWidth * 0.34);
    const double yTopBound = scene.plotRect.top() + 1.0;
    const double yBottomBound = scene.plotRect.bottom() - 1.0;
    const double xLeftBound = scene.plotRect.left() + boxHalfWidth + 1.0;
    const double xRightBound = scene.plotRect.right() - boxHalfWidth - 1.0;
    auto clamp_plot_y = [&](double y) { return qBound(yTopBound, y, yBottomBound); };

    for (int i = 0; i < dataSet.boxes.size(); ++i) {
        const BoxEntry& box = dataSet.boxes.at(i);
        const bool highlight = active_element(state, 0, i);
        double xCenter =
            map_linear(static_cast<double>(i), axis.xMin, axis.xMax, scene.plotRect.left(), scene.plotRect.right());
        if (compactHourLayout) {
            xCenter = qBound(xLeftBound, xCenter, xRightBound);
        }
        double bodyTop = clamp_plot_y(map_y_value(box.q3, axis, scene.plotRect));
        double bodyBottom = clamp_plot_y(map_y_value(box.q1, axis, scene.plotRect));
        if (bodyBottom < bodyTop) {
            std::swap(bodyTop, bodyBottom);
        }
        if ((bodyBottom - bodyTop) < 6.0) {
            const double centerY = clamp_plot_y(map_y_value(box.median, axis, scene.plotRect));
            bodyTop = qMax(yTopBound, centerY - 3.0);
            bodyBottom = qMin(yBottomBound, centerY + 3.0);
        }
        const QRectF body(xCenter - boxHalfWidth, bodyTop, bodyWidth, qMax(2.0, bodyBottom - bodyTop));
        const QColor fill =
            plotted_element_fill_color(box.color.isValid() ? box.color : QColor(QStringLiteral("#2d6cdf")), state, 0, i);
        append_rect(scene.fillVertices, body, alpha_color(fill, 170));
        append_rect_outline(scene.lineVertices, body, fill.darker(135));

        const double yMin = clamp_plot_y(map_y_value(box.minimum, axis, scene.plotRect));
        const double yMedian = clamp_plot_y(map_y_value(box.median, axis, scene.plotRect));
        const double yMax = clamp_plot_y(map_y_value(box.maximum, axis, scene.plotRect));
        append_line(scene.lineVertices, QPointF(xCenter, yMin), QPointF(xCenter, body.bottom()), fill.darker(135));
        append_line(scene.lineVertices, QPointF(xCenter, yMax), QPointF(xCenter, body.top()), fill.darker(135));
        append_line(scene.lineVertices, QPointF(xCenter - whiskerHalfWidth, yMin),
                    QPointF(xCenter + whiskerHalfWidth, yMin), fill.darker(135));
        append_line(scene.lineVertices, QPointF(xCenter - whiskerHalfWidth, yMax),
                    QPointF(xCenter + whiskerHalfWidth, yMax), fill.darker(135));
        append_line(scene.lineVertices, QPointF(body.left(), yMedian), QPointF(body.right(), yMedian),
                    fill.darker(180));

        if (box.sampleCount == 1) {
            const double radius = highlight ? 5.0 : 4.0;
            append_circle(scene.fillVertices, scene.lineVertices, QPointF(xCenter, yMedian), radius,
                          alpha_color(fill, 235), fill.darker(150), 18);
        }

        for (int outlierIndex = 0; outlierIndex < box.outliers.size(); ++outlierIndex) {
            const double yOutlier = clamp_plot_y(map_y_value(box.outliers.at(outlierIndex), axis, scene.plotRect));
            const double radius = highlight ? 4.2 : 3.2;
            append_circle(scene.fillVertices, scene.lineVertices, QPointF(xCenter, yOutlier), radius,
                          QColor(QStringLiteral("#ffffff")), fill.darker(135), 14);

            QPainterPath outlierPath;
            outlierPath.addEllipse(QPointF(xCenter, yOutlier), radius + 2.0, radius + 2.0);
            add_pick(scene, PickRole::Data, 0, i, outlierPath, outlierPath.boundingRect(), box.tooltip);
        }

        const double labelWidth = rotateLabels
                                      ? qMax(bodyWidth * 1.25,
                                             static_cast<double>(categoryMetrics.horizontalAdvance(box.label)) + 10.0)
                                      : qMax(bodyWidth * 1.35,
                                             static_cast<double>(categoryMetrics.horizontalAdvance(box.label)) + 10.0);
        const double labelHeight = categoryMetrics.height() + 4.0;
        const double labelCenterY = scene.plotRect.bottom() + 14.0 + (labelBand * 0.5);
        scene.labels.append(OverlayLabel{
            QRectF(xCenter - (labelWidth * 0.5), labelCenterY - (labelHeight * 0.5), labelWidth, labelHeight),
            box.label,
            QColor(QStringLiteral("#374151")),
            9.0,
            Qt::AlignHCenter | Qt::AlignVCenter,
            rotateLabels ? -45.0 : 0.0,
            false});

        QPainterPath path;
        path.addRect(body.adjusted(-3.0, -8.0, 3.0, 8.0));
        if (box.sampleCount == 1) {
            QPainterPath samplePath;
            samplePath.addEllipse(QPointF(xCenter, yMedian), 7.0, 7.0);
            path = path.united(samplePath);
        }
        add_pick(scene, PickRole::Data, 0, i, path, path.boundingRect(), box.tooltip);
    }
}

}  // namespace

ChartScene ChartLayoutEngine::build(const ChartDataSet& dataSet, const QSizeF& size, const ChartViewState& state) {
    ChartScene scene;
    scene.background = QColor(QStringLiteral("#f2f4f7"));
    scene.plotBackground = QColor(QStringLiteral("#fcfcfd"));

    if (size.width() <= 4.0 || size.height() <= 4.0) {
        return scene;
    }

    if (dataSet.kind == ChartKind::Empty || dataSet.isEmpty()) {
        scene.plotRect = QRectF(kLeftMargin, kTopMargin + 18.0, qMax(120.0, size.width() - kLeftMargin - kRightMargin),
                                qMax(100.0, size.height() - kTopMargin - kBottomMargin));
        add_plot_background(scene, dataSet);
        add_title_block(scene, dataSet, size);
        scene.labels.append(OverlayLabel{
            scene.plotRect.adjusted(18.0, 12.0, -18.0, -12.0),
            dataSet.subtitle.trimmed().isEmpty() ? QStringLiteral("Sem dados para exibir.") : dataSet.subtitle,
            QColor(QStringLiteral("#4b5563")),
            12.0,
            Qt::AlignCenter,
            0.0,
            false});
        return scene;
    }

    switch (dataSet.kind) {
        case ChartKind::Pie:
            build_pie(scene, dataSet, size, state);
            break;
        case ChartKind::Bars:
        case ChartKind::Histogram:
            build_bars(scene, dataSet, size, state, false);
            break;
        case ChartKind::Pareto:
        case ChartKind::ComboBarsLine:
            build_bars(scene, dataSet, size, state, true);
            break;
        case ChartKind::Timeline:
        case ChartKind::TemporalScatter:
        case ChartKind::Scatter:
        case ChartKind::DotPlot:
            build_scatter(scene, dataSet, size, state);
            break;
        case ChartKind::Radar:
            build_radar(scene, dataSet, size, state);
            break;
        case ChartKind::BoxPlot:
            build_boxplot(scene, dataSet, size, state);
            break;
        case ChartKind::Empty:
        default:
            break;
    }

    return scene;
}

}  // namespace source::charts_lgpl
