#pragma once

namespace source::services {
class DiretorioScanner;
}

namespace source::services::GerenciamentoDiretorioScanner {
void _atualizar_interface(source::services::DiretorioScanner* self, int progresso, int contador, int total);
}
