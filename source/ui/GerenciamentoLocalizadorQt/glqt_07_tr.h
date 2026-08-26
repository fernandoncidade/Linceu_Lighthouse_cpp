#pragma once

#include <QString>
#include <QStringList>

namespace source::ui {
class LocalizadorQt;
}

namespace source::ui::GerenciamentoLocalizadorQt {
QString tr(source::ui::LocalizadorQt* self, const QString& text, const QStringList& args);
}
