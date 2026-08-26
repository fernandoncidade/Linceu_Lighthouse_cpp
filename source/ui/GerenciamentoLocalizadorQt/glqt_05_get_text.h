#pragma once

#include <QString>
#include <QStringList>

namespace source::ui {
class LocalizadorQt;
}

namespace source::ui::GerenciamentoLocalizadorQt {
QString get_text(source::ui::LocalizadorQt* self, const QString& key, const QStringList& args);
}
