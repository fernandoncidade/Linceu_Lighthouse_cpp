#pragma once

#include <QSizeF>

#include "ch_07_LgplChartModel.h"

namespace source::charts_lgpl {

class ChartLayoutEngine {
public:
    static ChartScene build(const ChartDataSet& dataSet, const QSizeF& size, const ChartViewState& state);
};

}  // namespace source::charts_lgpl
