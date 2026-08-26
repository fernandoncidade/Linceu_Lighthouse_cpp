#pragma once

#include <QVariant>

#include "ch_07_LgplChartModel.h"

namespace source::charts_lgpl {

class LegacyVariantChartAdapter {
public:
    static bool supports(const QString& kind);
    static ChartDataSet adapt(const QString& fallbackTitle, const QVariant& figure);
};

}  // namespace source::charts_lgpl
