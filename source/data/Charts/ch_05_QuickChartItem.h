#pragma once

#include <QtQuick/QQuickItem>

#include "ch_01_ChartLayoutEngine.h"

namespace source::charts_lgpl {

class QuickChartItem : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(QVariant figure READ figure WRITE setFigure NOTIFY figureChanged)
    Q_PROPERTY(qreal transitionProgress READ transitionProgress WRITE setTransitionProgress NOTIFY transitionProgressChanged)
    Q_PROPERTY(QString hoverText READ hoverText NOTIFY hoverTextChanged)

public:
    explicit QuickChartItem(QQuickItem* parent = nullptr);

    QVariant figure() const;
    void setFigure(const QVariant& figure);

    qreal transitionProgress() const;
    void setTransitionProgress(qreal value);

    QString hoverText() const;

signals:
    void figureChanged();
    void transitionProgressChanged();
    void hoverTextChanged();

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;
    void hoverMoveEvent(QHoverEvent* event) override;
    void hoverLeaveEvent(QHoverEvent* event) override;

private:
    void rebuildScene();
    void updateHover(const QPointF& position);

    QVariant m_figure;
    ChartDataSet m_dataSet;
    ChartScene m_scene;
    ChartViewState m_state;
    QString m_hoverText;
    bool m_sceneDirty = true;
};

}  // namespace source::charts_lgpl
