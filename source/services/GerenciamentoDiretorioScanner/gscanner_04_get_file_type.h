#pragma once

#include <QString>

namespace source::services {
class DiretorioScanner;
}

namespace source::services::GerenciamentoDiretorioScanner {
QString get_file_type(source::services::DiretorioScanner* self, const QString& caminho);
}
