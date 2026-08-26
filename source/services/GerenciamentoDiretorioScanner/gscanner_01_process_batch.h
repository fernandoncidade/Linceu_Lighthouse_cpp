#pragma once

#include <QList>
#include <QVariantList>

namespace source::services {
class DiretorioScanner;
}

namespace source::services::GerenciamentoDiretorioScanner {
void _process_batch(source::services::DiretorioScanner* self, const QList<QVariantList>& batch);
}
