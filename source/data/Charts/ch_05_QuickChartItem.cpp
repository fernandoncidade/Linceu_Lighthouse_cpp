#include "ch_05_QuickChartItem.h"

#include <QHoverEvent>
#include <QtQuick/QSGGeometry>
#include <QtQuick/QSGGeometryNode>
#include <QtQuick/QSGNode>
#include <QtQuick/QSGVertexColorMaterial>

#include "ch_03_LegacyVariantChartAdapter.h"

namespace source::charts_lgpl {
namespace {

void copy_vertices(QSGGeometry& geometry, const QVector<ColoredVertex>& vertices) {
    auto* dst = geometry.vertexDataAsColoredPoint2D();
    for (int i = 0; i < vertices.size(); ++i) {
        const ColoredVertex& src = vertices.at(i);
        dst[i].x = src.x;
        dst[i].y = src.y;
        dst[i].r = static_cast<uchar>(qBound(0.0f, src.r, 1.0f) * 255.0f);
        dst[i].g = static_cast<uchar>(qBound(0.0f, src.g, 1.0f) * 255.0f);
        dst[i].b = static_cast<uchar>(qBound(0.0f, src.b, 1.0f) * 255.0f);
        dst[i].a = static_cast<uchar>(qBound(0.0f, src.a, 1.0f) * 255.0f);
    }
}

QSGGeometryNode* ensure_geometry_node(QSGNode* parent, int index, QSGGeometry::DrawingMode mode,
                                      const QVector<ColoredVertex>& vertices) {
    QSGGeometryNode* node = nullptr;
    if (parent->childCount() > index) {
        node = static_cast<QSGGeometryNode*>(parent->childAtIndex(index));
    } else {
        node = new QSGGeometryNode();
        auto* material = new QSGVertexColorMaterial();
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial, true);
        parent->appendChildNode(node);
    }

    auto* geometry = node->geometry();
    if (geometry == nullptr || geometry->vertexCount() != vertices.size()) {
        delete geometry;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), vertices.size());
        geometry->setDrawingMode(mode);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry, true);
    } else {
        geometry->setDrawingMode(mode);
    }

    copy_vertices(*geometry, vertices);
    node->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
    return node;
}

}  // namespace

QuickChartItem::QuickChartItem(QQuickItem* parent) : QQuickItem(parent) {
    setFlag(ItemHasContents, true);
    setAcceptHoverEvents(true);
}

QVariant QuickChartItem::figure() const { return m_figure; }

void QuickChartItem::setFigure(const QVariant& figure) {
    if (m_figure == figure) {
        return;
    }
    m_figure = figure;
    m_dataSet = LegacyVariantChartAdapter::adapt(QString(), figure);
    m_sceneDirty = true;
    update();
    emit figureChanged();
}

qreal QuickChartItem::transitionProgress() const { return static_cast<qreal>(m_state.animationT); }

void QuickChartItem::setTransitionProgress(qreal value) {
    const double clamped = qBound(0.0, static_cast<double>(value), 1.0);
    if (qFuzzyCompare(m_state.animationT, clamped)) {
        return;
    }
    m_state.animationT = clamped;
    m_sceneDirty = true;
    update();
    emit transitionProgressChanged();
}

QString QuickChartItem::hoverText() const { return m_hoverText; }

void QuickChartItem::rebuildScene() {
    if (!m_sceneDirty) {
        return;
    }
    m_scene = ChartLayoutEngine::build(m_dataSet, QSizeF(width(), height()), m_state);
    m_sceneDirty = false;
}

QSGNode* QuickChartItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {
    rebuildScene();
    auto* root = oldNode != nullptr ? oldNode : new QSGNode();
    ensure_geometry_node(root, 0, QSGGeometry::DrawTriangles, m_scene.fillVertices);
    ensure_geometry_node(root, 1, QSGGeometry::DrawLines, m_scene.lineVertices);
    ensure_geometry_node(root, 2, QSGGeometry::DrawTriangles, m_scene.foregroundFillVertices);
    ensure_geometry_node(root, 3, QSGGeometry::DrawLines, m_scene.foregroundLineVertices);
    return root;
}

void QuickChartItem::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) {
    QQuickItem::geometryChange(newGeometry, oldGeometry);
    m_sceneDirty = true;
    update();
}

void QuickChartItem::updateHover(const QPointF& position) {
    QString hover;
    int hoveredSeries = -1;
    int hoveredElement = -1;
    int hoveredLegend = -1;
    for (int i = m_scene.picks.size() - 1; i >= 0; --i) {
        const PickItem& pick = m_scene.picks.at(i);
        if (!pick.contains(position)) {
            continue;
        }
        hover = pick.tooltip;
        if (pick.role == PickRole::Legend) {
            hoveredLegend = pick.seriesIndex;
            hoveredSeries = pick.seriesIndex;
        } else if (pick.role == PickRole::Data) {
            hoveredSeries = pick.seriesIndex;
            hoveredElement = pick.elementIndex;
        }
        break;
    }

    const bool changed = hover != m_hoverText || hoveredSeries != m_state.hoveredSeries ||
                         hoveredElement != m_state.hoveredElement || hoveredLegend != m_state.hoveredLegend;
    if (!changed) {
        return;
    }
    m_hoverText = hover;
    m_state.hoveredSeries = hoveredSeries;
    m_state.hoveredElement = hoveredElement;
    m_state.hoveredLegend = hoveredLegend;
    m_sceneDirty = true;
    update();
    emit hoverTextChanged();
}

void QuickChartItem::hoverMoveEvent(QHoverEvent* event) {
    rebuildScene();
    updateHover(event->position());
    QQuickItem::hoverMoveEvent(event);
}

void QuickChartItem::hoverLeaveEvent(QHoverEvent* event) {
    updateHover(QPointF(-1000.0, -1000.0));
    QQuickItem::hoverLeaveEvent(event);
}

}  // namespace source::charts_lgpl
