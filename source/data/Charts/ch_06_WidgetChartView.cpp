#include "ch_06_WidgetChartView.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QSurfaceFormat>
#include <QToolTip>
#include <QWheelEvent>

namespace source::charts_lgpl {
namespace {

bool same_shape(const ChartDataSet& left, const ChartDataSet& right) {
    if (left.kind != right.kind || left.series.size() != right.series.size() || left.boxes.size() != right.boxes.size()) {
        return false;
    }
    if (left.categories != right.categories) {
        return false;
    }
    for (int i = 0; i < left.series.size(); ++i) {
        if (left.series.at(i).points.size() != right.series.at(i).points.size() ||
            left.series.at(i).role != right.series.at(i).role) {
            return false;
        }
    }
    for (int i = 0; i < left.boxes.size(); ++i) {
        if (left.boxes.at(i).outliers.size() != right.boxes.at(i).outliers.size()) {
            return false;
        }
    }
    return true;
}

ChartDataSet reveal_data_set(const ChartDataSet& source, double t) {
    ChartDataSet revealed = source;
    for (ChartSeries& series : revealed.series) {
        for (ChartPoint& point : series.points) {
            point.y *= t;
            point.y2 *= t;
        }
    }
    for (BoxEntry& box : revealed.boxes) {
        box.minimum *= t;
        box.q1 *= t;
        box.median *= t;
        box.q3 *= t;
        box.maximum *= t;
        for (double& outlier : box.outliers) {
            outlier *= t;
        }
    }
    return revealed;
}

ChartDataSet interpolate(const ChartDataSet& from, const ChartDataSet& to, double t) {
    if (t >= 1.0) {
        return to;
    }
    if (t <= 0.0) {
        return from;
    }
    if (!same_shape(from, to)) {
        return reveal_data_set(to, t);
    }

    ChartDataSet blended = to;
    for (int seriesIndex = 0; seriesIndex < blended.series.size(); ++seriesIndex) {
        for (int pointIndex = 0; pointIndex < blended.series.at(seriesIndex).points.size(); ++pointIndex) {
            ChartPoint point = blended.series.at(seriesIndex).points.at(pointIndex);
            const ChartPoint& oldPoint = from.series.at(seriesIndex).points.at(pointIndex);
            point.x = oldPoint.x + ((point.x - oldPoint.x) * t);
            point.y = oldPoint.y + ((point.y - oldPoint.y) * t);
            point.y2 = oldPoint.y2 + ((point.y2 - oldPoint.y2) * t);
            blended.series[seriesIndex].points[pointIndex] = point;
        }
    }

    for (int boxIndex = 0; boxIndex < blended.boxes.size(); ++boxIndex) {
        BoxEntry box = blended.boxes.at(boxIndex);
        const BoxEntry& oldBox = from.boxes.at(boxIndex);
        box.minimum = oldBox.minimum + ((box.minimum - oldBox.minimum) * t);
        box.q1 = oldBox.q1 + ((box.q1 - oldBox.q1) * t);
        box.median = oldBox.median + ((box.median - oldBox.median) * t);
        box.q3 = oldBox.q3 + ((box.q3 - oldBox.q3) * t);
        box.maximum = oldBox.maximum + ((box.maximum - oldBox.maximum) * t);
        for (int outlierIndex = 0; outlierIndex < qMin(box.outliers.size(), oldBox.outliers.size()); ++outlierIndex) {
            box.outliers[outlierIndex] =
                oldBox.outliers[outlierIndex] + ((box.outliers[outlierIndex] - oldBox.outliers[outlierIndex]) * t);
        }
        blended.boxes[boxIndex] = box;
    }

    return blended;
}

double dataset_min_x(const ChartDataSet& dataSet) {
    if (dataSet.kind == ChartKind::Bars || dataSet.kind == ChartKind::Histogram || dataSet.kind == ChartKind::Pareto ||
        dataSet.kind == ChartKind::ComboBarsLine || dataSet.kind == ChartKind::DotPlot || dataSet.kind == ChartKind::BoxPlot) {
        return -0.5;
    }

    double minValue = std::numeric_limits<double>::max();
    for (const ChartSeries& series : dataSet.series) {
        for (const ChartPoint& point : series.points) {
            minValue = qMin(minValue, point.x);
        }
    }
    return std::isfinite(minValue) ? minValue : 0.0;
}

double dataset_max_x(const ChartDataSet& dataSet) {
    if (dataSet.kind == ChartKind::Bars || dataSet.kind == ChartKind::Histogram || dataSet.kind == ChartKind::Pareto ||
        dataSet.kind == ChartKind::ComboBarsLine || dataSet.kind == ChartKind::DotPlot) {
        return qMax(0.5, static_cast<double>(qMax(1, dataSet.categories.size())) - 0.5);
    }
    if (dataSet.kind == ChartKind::BoxPlot) {
        return qMax(0.5, static_cast<double>(qMax(1, dataSet.boxes.size())) - 0.5);
    }

    double maxValue = std::numeric_limits<double>::lowest();
    for (const ChartSeries& series : dataSet.series) {
        for (const ChartPoint& point : series.points) {
            maxValue = qMax(maxValue, point.x);
        }
    }
    return std::isfinite(maxValue) ? maxValue : 1.0;
}

bool can_pan_zoom(const ChartKind kind) {
    switch (kind) {
        case ChartKind::Bars:
        case ChartKind::Histogram:
        case ChartKind::Pareto:
        case ChartKind::ComboBarsLine:
        case ChartKind::Timeline:
        case ChartKind::TemporalScatter:
        case ChartKind::Scatter:
        case ChartKind::DotPlot:
            return true;
        default:
            return false;
    }
}

}  // namespace

WidgetChartView::WidgetChartView(QWidget* parent)
    : QOpenGLWidget(parent),
      m_fillVbo(QOpenGLBuffer::VertexBuffer),
      m_lineVbo(QOpenGLBuffer::VertexBuffer),
      m_foregroundFillVbo(QOpenGLBuffer::VertexBuffer),
      m_foregroundLineVbo(QOpenGLBuffer::VertexBuffer) {
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setSamples(qMax(8, format.samples()));
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    setFormat(format);

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    setMinimumHeight(360);
    setObjectName(QStringLiteral("lgplChartWidget"));

    m_animation.setDuration(420);
    m_animation.setStartValue(0.0);
    m_animation.setEndValue(1.0);
    m_animation.setEasingCurve(QEasingCurve::InOutCubic);
    QObject::connect(&m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
        setAnimationValue(value.toReal());
    });
    QObject::connect(&m_animation, &QVariantAnimation::finished, this, [this]() {
        m_previousData = m_currentData;
        m_state.animationT = 1.0;
        m_sceneDirty = true;
        update();
    });
}

WidgetChartView::~WidgetChartView() {
    if (context() != nullptr) {
        makeCurrent();
        m_fillVbo.destroy();
        m_lineVbo.destroy();
        m_foregroundFillVbo.destroy();
        m_foregroundLineVbo.destroy();
        doneCurrent();
    }
}

void WidgetChartView::setDataSet(const ChartDataSet& dataSet) {
    const bool sameKind = (m_currentData.kind == dataSet.kind);
    m_previousData = m_currentData;
    m_currentData = dataSet;
    if (!sameKind) {
        resetDataWindow();
    }

    m_sceneDirty = true;
    m_buffersDirty = true;

    if (m_exportMode || !m_animationsEnabled) {
        m_state.animationT = 1.0;
        update();
        return;
    }

    m_state.animationT = 0.0;
    m_animation.stop();
    m_animation.setDuration(dataSet.meta.value(QStringLiteral("realtime")).toBool() ? 140 : 420);
    m_animation.start();
    update();
}

const ChartDataSet& WidgetChartView::dataSet() const { return m_currentData; }

void WidgetChartView::setAnimationsEnabled(bool enabled) {
    m_animationsEnabled = enabled;
    if (!enabled) {
        m_animation.stop();
        m_state.animationT = 1.0;
        m_sceneDirty = true;
        update();
    }
}

bool WidgetChartView::animationsEnabled() const { return m_animationsEnabled; }

void WidgetChartView::setExportMode(bool enabled) {
    m_exportMode = enabled;
    m_state.exportMode = enabled;
    if (enabled) {
        setAnimationsEnabled(false);
    }
    m_sceneDirty = true;
}

bool WidgetChartView::exportMode() const { return m_exportMode; }

qreal WidgetChartView::animationValue() const { return static_cast<qreal>(m_state.animationT); }

void WidgetChartView::setAnimationValue(qreal value) {
    m_state.animationT = qBound(0.0, static_cast<double>(value), 1.0);
    m_sceneDirty = true;
    m_buffersDirty = true;
    update();
}

void WidgetChartView::initializeGL() {
    initializeOpenGLFunctions();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifdef GL_MULTISAMPLE
    glEnable(GL_MULTISAMPLE);
#endif
#ifdef GL_LINE_SMOOTH
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
#endif

    m_program.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                      "attribute highp vec2 position;\n"
                                      "attribute highp vec4 color;\n"
                                      "uniform highp vec2 viewport;\n"
                                      "uniform highp float scaleFactor;\n"
                                      "varying highp vec4 vColor;\n"
                                      "void main() {\n"
                                      "  highp vec2 scaled = position * scaleFactor;\n"
                                      "  highp vec2 ndc = vec2((scaled.x / viewport.x) * 2.0 - 1.0,\n"
                                      "                        1.0 - (scaled.y / viewport.y) * 2.0);\n"
                                      "  gl_Position = vec4(ndc, 0.0, 1.0);\n"
                                      "  vColor = color;\n"
                                      "}\n");
    m_program.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                      "varying highp vec4 vColor;\n"
                                      "void main() {\n"
                                      "  gl_FragColor = vColor;\n"
                                      "}\n");
    m_program.link();

    m_fillVbo.create();
    m_lineVbo.create();
    m_foregroundFillVbo.create();
    m_foregroundLineVbo.create();
}

void WidgetChartView::resizeGL(int, int) {
    m_sceneDirty = true;
    m_buffersDirty = true;
}

void WidgetChartView::ensureScene() {
    if (!m_sceneDirty) {
        return;
    }
    m_scene = ChartLayoutEngine::build(interpolatedDataSet(), QSizeF(width(), height()), m_state);
    m_sceneDirty = false;
    m_buffersDirty = true;
}

ChartDataSet WidgetChartView::interpolatedDataSet() const {
    if (m_exportMode || !m_animationsEnabled) {
        return m_currentData;
    }
    return interpolate(m_previousData, m_currentData, m_state.animationT);
}

void WidgetChartView::uploadBuffersIfNeeded() {
    if (!m_buffersDirty) {
        return;
    }

    if (!m_fillVbo.isCreated() || !m_lineVbo.isCreated() || !m_foregroundFillVbo.isCreated() ||
        !m_foregroundLineVbo.isCreated()) {
        return;
    }

    auto upload = [](QOpenGLBuffer& buffer, const QVector<ColoredVertex>& vertices) {
        buffer.bind();
        buffer.allocate(vertices.constData(), static_cast<int>(vertices.size() * sizeof(ColoredVertex)));
        buffer.release();
    };

    upload(m_fillVbo, m_scene.fillVertices);
    upload(m_lineVbo, m_scene.lineVertices);
    upload(m_foregroundFillVbo, m_scene.foregroundFillVertices);
    upload(m_foregroundLineVbo, m_scene.foregroundLineVertices);

    m_buffersDirty = false;
}

void WidgetChartView::paintGL() {
    ensureScene();
    uploadBuffersIfNeeded();

    const QColor clear = m_scene.background.isValid() ? m_scene.background : QColor(QStringLiteral("#f2f4f7"));
    glViewport(0, 0, int(width() * devicePixelRatioF()), int(height() * devicePixelRatioF()));
    glClearColor(clear.redF(), clear.greenF(), clear.blueF(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!m_program.isLinked()) {
        return;
    }

    m_program.bind();
    m_program.setUniformValue("viewport", QVector2D(width() * devicePixelRatioF(), height() * devicePixelRatioF()));
    m_program.setUniformValue("scaleFactor", static_cast<float>(devicePixelRatioF()));

    const int positionLoc = m_program.attributeLocation("position");
    const int colorLoc = m_program.attributeLocation("color");

    auto drawVertices = [&](QOpenGLBuffer& buffer, const QVector<ColoredVertex>& vertices, GLenum mode) {
        if (vertices.isEmpty()) {
            return;
        }
        buffer.bind();
        m_program.enableAttributeArray(positionLoc);
        m_program.enableAttributeArray(colorLoc);
        m_program.setAttributeBuffer(positionLoc, GL_FLOAT, offsetof(ColoredVertex, x), 2, sizeof(ColoredVertex));
        m_program.setAttributeBuffer(colorLoc, GL_FLOAT, offsetof(ColoredVertex, r), 4, sizeof(ColoredVertex));
        glDrawArrays(mode, 0, static_cast<GLsizei>(vertices.size()));
        buffer.release();
    };

    drawVertices(m_fillVbo, m_scene.fillVertices, GL_TRIANGLES);
    drawVertices(m_lineVbo, m_scene.lineVertices, GL_LINES);
    drawVertices(m_foregroundFillVbo, m_scene.foregroundFillVertices, GL_TRIANGLES);
    drawVertices(m_foregroundLineVbo, m_scene.foregroundLineVertices, GL_LINES);

    m_program.disableAttributeArray(positionLoc);
    m_program.disableAttributeArray(colorLoc);
    m_program.release();

    paintOverlay();
}

void WidgetChartView::paintOverlay() {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    for (const LegendEntry& entry : m_scene.legends) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(entry.color);
        const qreal swatchSize = qBound<qreal>(6.0, entry.rect.height() - 6.0, 12.0);
        const QRectF swatchRect(entry.rect.left(), entry.rect.center().y() - (swatchSize * 0.5), swatchSize, swatchSize);
        painter.drawRoundedRect(swatchRect, 3.0, 3.0);

        QFont font = painter.font();
        font.setPointSizeF(entry.fontSize);
        const bool sameHoveredSeries = (entry.seriesIndex == m_state.hoveredSeries);
        const bool sameHoveredLegend = (entry.seriesIndex == m_state.hoveredLegend);
        const bool sameHoveredElement = (entry.elementIndex < 0 || entry.elementIndex == m_state.hoveredElement);
        const bool sameSelectedSeries = (entry.seriesIndex == m_state.selectedSeries);
        const bool sameSelectedElement = (entry.elementIndex < 0 || entry.elementIndex == m_state.selectedElement);
        font.setBold((sameHoveredLegend && sameHoveredElement) || (sameHoveredSeries && sameHoveredElement) ||
                     (sameSelectedSeries && sameSelectedElement));
        painter.setFont(font);
        painter.setPen(QColor(QStringLiteral("#1f2937")));
        painter.drawText(entry.rect.adjusted(swatchSize + 6.0, 0.0, -4.0, 0.0), Qt::AlignLeft | Qt::AlignVCenter, entry.label);
    }

    for (const OverlayLabel& label : m_scene.labels) {
        painter.save();
        QFont font = painter.font();
        font.setPointSizeF(label.fontSize);
        font.setBold(label.bold);
        painter.setFont(font);
        painter.setPen(label.color);
        if (std::abs(label.rotation) > 0.1) {
            painter.translate(label.rect.center());
            painter.rotate(label.rotation);
            painter.drawText(QRectF(-label.rect.width() * 0.5, -label.rect.height() * 0.5, label.rect.width(),
                                    label.rect.height()),
                             label.alignment, label.text);
        } else {
            painter.drawText(label.rect, label.alignment, label.text);
        }
        painter.restore();
    }
}

const PickItem* WidgetChartView::pickAt(const QPointF& position) const {
    for (int i = m_scene.picks.size() - 1; i >= 0; --i) {
        const PickItem& pick = m_scene.picks.at(i);
        if (pick.contains(position)) {
            return &pick;
        }
    }
    return nullptr;
}

void WidgetChartView::updateHoverState(const QPointF& position) {
    const PickItem* pick = pickAt(position);
    int hoveredSeries = -1;
    int hoveredElement = -1;
    int hoveredLegend = -1;
    QString tooltip;

    if (pick != nullptr) {
        if (pick->role == PickRole::Data) {
            hoveredSeries = pick->seriesIndex;
            hoveredElement = pick->elementIndex;
            tooltip = pick->tooltip;
        } else if (pick->role == PickRole::Legend) {
            hoveredLegend = pick->seriesIndex;
            hoveredSeries = pick->seriesIndex;
            hoveredElement = pick->elementIndex;
            tooltip = pick->tooltip;
        }
    }

    const bool changed = hoveredSeries != m_state.hoveredSeries || hoveredElement != m_state.hoveredElement ||
                         hoveredLegend != m_state.hoveredLegend;
    if (!changed) {
        if (!tooltip.isEmpty() && !QToolTip::isVisible()) {
            QToolTip::showText(QCursor::pos(), tooltip, this);
        }
        return;
    }

    m_state.hoveredSeries = hoveredSeries;
    m_state.hoveredElement = hoveredElement;
    m_state.hoveredLegend = hoveredLegend;
    m_sceneDirty = true;
    update();

    if (!tooltip.isEmpty()) {
        QToolTip::showText(QCursor::pos(), tooltip, this);
    } else {
        QToolTip::hideText();
    }
}

void WidgetChartView::clearHoverState() {
    if (m_state.hoveredSeries < 0 && m_state.hoveredElement < 0 && m_state.hoveredLegend < 0) {
        return;
    }
    m_state.hoveredSeries = -1;
    m_state.hoveredElement = -1;
    m_state.hoveredLegend = -1;
    m_sceneDirty = true;
    update();
    QToolTip::hideText();
}

void WidgetChartView::applySelectionAt(const QPointF& position) {
    const PickItem* pick = pickAt(position);
    int selectedSeries = -1;
    int selectedElement = -1;
    if (pick != nullptr && pick->role == PickRole::Data) {
        selectedSeries = pick->seriesIndex;
        selectedElement = pick->elementIndex;
    } else if (pick != nullptr && pick->role == PickRole::Legend) {
        selectedSeries = pick->seriesIndex;
        selectedElement = pick->elementIndex;
    }

    const bool changed = selectedSeries != m_state.selectedSeries || selectedElement != m_state.selectedElement;
    m_state.selectedSeries = selectedSeries;
    m_state.selectedElement = selectedElement;
    if (changed) {
        m_sceneDirty = true;
        update();
        emit selectionChanged(selectedSeries, selectedElement);
    }
}

bool WidgetChartView::supportsPanZoom() const {
    return can_pan_zoom(m_currentData.kind);
}

QRectF WidgetChartView::fullDataWindowFor(const ChartDataSet& dataSet) const {
    return QRectF(QPointF(dataset_min_x(dataSet), 0.0), QPointF(dataset_max_x(dataSet), 1.0)).normalized();
}

QRectF WidgetChartView::clampedDataWindow(const QRectF& proposed) const {
    const QRectF full = fullDataWindowFor(m_currentData);
    QRectF window = proposed.normalized();
    if (window.width() < 0.5) {
        window.setWidth(0.5);
    }
    if (window.left() < full.left()) {
        window.moveLeft(full.left());
    }
    if (window.right() > full.right()) {
        window.moveRight(full.right());
    }
    if (window.width() > full.width()) {
        window = full;
    }
    return window;
}

void WidgetChartView::resetDataWindow() {
    m_state.hasDataWindow = false;
    m_sceneDirty = true;
}

void WidgetChartView::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging && supportsPanZoom()) {
        const QRectF full = fullDataWindowFor(m_currentData);
        QRectF base = m_dragStartWindow.isValid() ? m_dragStartWindow : full;
        if (!m_state.hasDataWindow) {
            m_state.dataWindow = full;
            m_state.hasDataWindow = true;
            base = full;
        }

        const double pixels = event->position().x() - m_dragStartPosition.x();
        const double dataSpan = base.width();
        const double plotWidth = qMax(1.0, m_scene.plotRect.width());
        const double delta = -(pixels / plotWidth) * dataSpan;
        QRectF shifted(base.left() + delta, 0.0, dataSpan, 1.0);
        m_state.dataWindow = clampedDataWindow(shifted);
        m_sceneDirty = true;
        update();
    } else {
        updateHoverState(event->position());
    }
    m_lastMousePosition = event->position();
    QOpenGLWidget::mouseMoveEvent(event);
}

void WidgetChartView::leaveEvent(QEvent* event) {
    clearHoverState();
    QOpenGLWidget::leaveEvent(event);
}

void WidgetChartView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        ensureScene();
        if (supportsPanZoom() && m_scene.plotRect.contains(event->position())) {
            m_dragging = true;
            m_dragStartPosition = event->position();
            m_dragStartWindow = m_state.hasDataWindow ? m_state.dataWindow : fullDataWindowFor(m_currentData);
        }
        applySelectionAt(event->position());
    }
    QOpenGLWidget::mousePressEvent(event);
}

void WidgetChartView::mouseReleaseEvent(QMouseEvent* event) {
    m_dragging = false;
    QOpenGLWidget::mouseReleaseEvent(event);
}

void WidgetChartView::wheelEvent(QWheelEvent* event) {
    if (!supportsPanZoom()) {
        QOpenGLWidget::wheelEvent(event);
        return;
    }

    ensureScene();
    if (!m_scene.plotRect.contains(event->position())) {
        QOpenGLWidget::wheelEvent(event);
        return;
    }

    const QRectF full = fullDataWindowFor(m_currentData);
    QRectF window = m_state.hasDataWindow ? m_state.dataWindow : full;
    const double plotRatio =
        qBound(0.0, (event->position().x() - m_scene.plotRect.left()) / qMax(1.0, m_scene.plotRect.width()), 1.0);
    const double focusX = window.left() + (window.width() * plotRatio);
    const double factor = event->angleDelta().y() > 0 ? 0.85 : 1.18;
    const double newWidth = qBound(0.5, window.width() * factor, full.width());
    const double newLeft = focusX - (newWidth * plotRatio);
    window = clampedDataWindow(QRectF(newLeft, 0.0, newWidth, 1.0));
    m_state.dataWindow = window;
    m_state.hasDataWindow = (std::abs(window.width() - full.width()) > 0.001);
    m_sceneDirty = true;
    update();
    event->accept();
}

}  // namespace source::charts_lgpl
