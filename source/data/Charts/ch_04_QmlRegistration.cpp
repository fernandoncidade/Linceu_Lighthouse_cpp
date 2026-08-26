#include "ch_04_QmlRegistration.h"

#include <QtQml/qqml.h>

#include "ch_05_QuickChartItem.h"

namespace source::charts_lgpl {

void register_qml_types() {
    qmlRegisterType<QuickChartItem>("Linceu.Charts", 1, 0, "QuickChartItem");
}

}  // namespace source::charts_lgpl
