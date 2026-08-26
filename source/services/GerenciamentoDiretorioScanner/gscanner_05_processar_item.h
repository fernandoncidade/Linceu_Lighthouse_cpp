#pragma once

#include <QString>

namespace source::services {
class DiretorioScanner;
}

namespace source::services::GerenciamentoDiretorioScanner {
void _processar_item(source::services::DiretorioScanner* self, const QString& nome, const QString& caminho, const QString& tipo);
}
