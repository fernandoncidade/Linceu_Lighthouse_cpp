#pragma once

#include <QColor>
#include <QMetaType>
#include <QPainterPath>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QVector>

namespace source::charts_lgpl {

enum class ChartKind {
    Empty,
    Pie,
    Bars,
    Histogram,
    Pareto,
    ComboBarsLine,
    Timeline,
    TemporalScatter,
    Scatter,
    DotPlot,
    Radar,
    BoxPlot
};

struct ChartPoint {
    double x = 0.0;
    double y = 0.0;
    double y2 = 0.0;
    QString label;
    QString tooltip;
    QColor color;
    QVariantMap meta;
};

struct ChartSeries {
    QString name;
    QString role;
    QColor color;
    QVector<ChartPoint> points;
    QVariantMap meta;
};

struct BoxEntry {
    QString label;
    QString tooltip;
    QColor color;
    int sampleCount = 0;
    double minimum = 0.0;
    double q1 = 0.0;
    double median = 0.0;
    double q3 = 0.0;
    double maximum = 0.0;
    QVector<double> outliers;
    QVariantMap meta;
};

struct ChartDataSet {
    ChartKind kind = ChartKind::Empty;
    QString title;
    QString subtitle;
    QString xLabel;
    QString yLabel;
    QString y2Label;
    QStringList categories;
    QVector<ChartSeries> series;
    QVector<BoxEntry> boxes;
    QVariantMap meta;

    [[nodiscard]] bool isEmpty() const {
        return kind == ChartKind::Empty ||
               ((series.isEmpty() || series.constFirst().points.isEmpty()) && boxes.isEmpty());
    }
};

struct ColoredVertex {
    float x = 0.0f;
    float y = 0.0f;
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};

struct OverlayLabel {
    QRectF rect;
    QString text;
    QColor color = Qt::black;
    qreal fontSize = 11.0;
    Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter;
    qreal rotation = 0.0;
    bool bold = false;
};

enum class PickRole {
    None,
    PlotArea,
    Data,
    Legend
};

struct PickItem {
    PickRole role = PickRole::None;
    int seriesIndex = -1;
    int elementIndex = -1;
    QRectF bounds;
    QPainterPath path;
    QString tooltip;
    QVariantMap meta;

    [[nodiscard]] bool contains(const QPointF& point) const {
        if (!bounds.adjusted(-3.0, -3.0, 3.0, 3.0).contains(point)) {
            return false;
        }
        return path.isEmpty() ? bounds.contains(point) : path.contains(point);
    }
};

struct LegendEntry {
    QRectF rect;
    QString label;
    QColor color = Qt::black;
    int seriesIndex = -1;
    int elementIndex = -1;
    qreal fontSize = 9.5;
};

struct ChartScene {
    QColor background = QColor(QStringLiteral("#f5f6f8"));
    QColor plotBackground = QColor(Qt::white);
    QRectF plotRect;
    QVector<ColoredVertex> fillVertices;
    QVector<ColoredVertex> lineVertices;
    QVector<OverlayLabel> labels;
    QVector<LegendEntry> legends;
    QVector<PickItem> picks;
};

struct ChartViewState {
    double animationT = 1.0;
    int hoveredSeries = -1;
    int hoveredElement = -1;
    int hoveredLegend = -1;
    int selectedSeries = -1;
    int selectedElement = -1;
    QRectF dataWindow;
    bool hasDataWindow = false;
    bool exportMode = false;
};

}  // namespace source::charts_lgpl

Q_DECLARE_METATYPE(source::charts_lgpl::ChartDataSet)
