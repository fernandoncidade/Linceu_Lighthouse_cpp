#pragma once

#include <QHash>
#include <QString>

namespace source::ui {
class LocalizadorQt;
}

namespace source::ui::GerenciamentoLocalizadorQt {
QHash<QString, QString> get_idiomas_disponiveis(source::ui::LocalizadorQt* self);
}
