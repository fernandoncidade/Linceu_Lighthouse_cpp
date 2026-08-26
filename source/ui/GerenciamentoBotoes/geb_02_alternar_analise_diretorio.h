#pragma once

class QThread;

namespace source::ui {
class GerenciadorBotoes;
}

namespace source::ui::GerenciamentoBotoes {

bool is_thread_valid_and_running(QThread* thread);
void alternar_analise_diretorio(source::ui::GerenciadorBotoes* self);

}  // namespace source::ui::GerenciamentoBotoes
