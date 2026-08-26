#pragma once

#include <QVariantMap>

namespace source::ui {
class LocalizadorQt;
}

namespace source::ui::GerenciamentoLocalizadorQt {
QVariantMap criar_mapa_compatibilidade(source::ui::LocalizadorQt* self);
}
