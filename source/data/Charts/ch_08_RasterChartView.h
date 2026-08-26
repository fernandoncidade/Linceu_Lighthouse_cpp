#pragma once

#include <QVariantAnimation>
#include <QWidget>

#include "ch_01_ChartLayoutEngine.h"

namespace source::charts_lgpl {

class RasterChartView final : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal animationValue READ animationValue WRITE setAnimationValue)

public:
    explicit RasterChartView(QWidget* parent = nullptr);

    void setDataSet(const ChartDataSet& dataSet);
    [[nodiscard]] const ChartDataSet& dataSet() const;

    void setAnimationsEnabled(bool enabled);
    [[nodiscard]] bool animationsEnabled() const;

    void setExportMode(bool enabled);
    [[nodiscard]] bool exportMode() const;

    [[nodiscard]] qreal animationValue() const;
    void setAnimationValue(qreal value);

signals:
    void selectionChanged(int seriesIndex, int elementIndex);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void ensureScene();
    void paintScene(QPainter& painter);
    void updateHoverState(const QPointF& position);
    void clearHoverState();
    void applySelectionAt(const QPointF& position);
    void resetDataWindow();
    bool supportsPanZoom() const;

    QRectF fullDataWindowFor(const ChartDataSet& dataSet) const;
    QRectF clampedDataWindow(const QRectF& proposed) const;
    ChartDataSet interpolatedDataSet() const;
    const PickItem* pickAt(const QPointF& position) const;

    ChartDataSet m_currentData;
    ChartDataSet m_previousData;
    ChartScene m_scene;
    ChartViewState m_state;
    bool m_sceneDirty = true;
    bool m_animationsEnabled = true;
    bool m_exportMode = false;
    bool m_dragging = false;
    QPointF m_lastMousePosition;
    QRectF m_dragStartWindow;
    QPointF m_dragStartPosition;

    QVariantAnimation m_animation;
};

}  // namespace source::charts_lgpl
