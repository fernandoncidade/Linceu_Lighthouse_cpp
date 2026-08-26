#pragma once

#include "gst_01_BaseGerador.h"

namespace source::data {
class GraficoSankey : public BaseGerador {
public:
    using BaseGerador::BaseGerador;
    QVariant gerar() override;
};
}
