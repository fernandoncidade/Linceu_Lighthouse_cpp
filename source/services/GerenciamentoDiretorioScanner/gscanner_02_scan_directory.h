#pragma once

#include <QString>

namespace source::services {
class DiretorioScanner;
}

namespace source::services::GerenciamentoDiretorioScanner {
void scan_directory(source::services::DiretorioScanner* self, const QString& directory);
}
