#pragma once

#include <QtGui/QOpenGLFunctions>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QVariantAnimation>

#include "ch_01_ChartLayoutEngine.h"

namespace source::charts_lgpl {

class WidgetChartView final : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
    Q_PROPERTY(qreal animationValue READ animationValue WRITE setAnimationValue)

public:
    explicit WidgetChartView(QWidget* parent = nullptr);
    ~WidgetChartView() override;

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
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void ensureScene();
    void updateHoverState(const QPointF& position);
    void clearHoverState();
    void applySelectionAt(const QPointF& position);
    void resetDataWindow();
    bool supportsPanZoom() const;

    QRectF fullDataWindowFor(const ChartDataSet& dataSet) const;
    QRectF clampedDataWindow(const QRectF& proposed) const;
    ChartDataSet interpolatedDataSet() const;
    void uploadBuffersIfNeeded();
    void paintOverlay();

    const PickItem* pickAt(const QPointF& position) const;

    ChartDataSet m_currentData;
    ChartDataSet m_previousData;
    ChartScene m_scene;
    ChartViewState m_state;
    bool m_sceneDirty = true;
    bool m_buffersDirty = true;
    bool m_animationsEnabled = true;
    bool m_exportMode = false;
    bool m_dragging = false;
    QPointF m_lastMousePosition;
    QRectF m_dragStartWindow;
    QPointF m_dragStartPosition;

    QVariantAnimation m_animation;

    QOpenGLShaderProgram m_program;
    QOpenGLBuffer m_fillVbo;
    QOpenGLBuffer m_lineVbo;
};

}  // namespace source::charts_lgpl
