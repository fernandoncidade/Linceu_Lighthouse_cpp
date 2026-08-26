#include "gob_11_ProcessarEvento.h"

#include <QDir>
#include <QFileInfo>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_04_EventoAdicionado.h"
#include "source/services/ob_05_EventoExcluido.h"
#include "source/services/ob_06_EventoModificado.h"
#include "source/services/ob_07_EventoRenomeado.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoObservador {

void processar_evento(source::services::Observador* self, const QVariantMap& evento) {
    try {
        if (!self || self->pausado || !self->pause_event_set) return;

        if (self->falhas_consecutivas >= self->max_falhas) {
            self->reiniciar_monitoramento();
            return;
        }

        const int acao = evento.value("acao").toInt();
        const QString nome_arquivo = evento.value("nome_arquivo").toString();
        const double tempo_atual = evento.value("tempo_evento").toDouble();
        const QString caminho_completo = QDir(self->diretorio).filePath(nome_arquivo);

        if (self->eventos_ignorados.contains(nome_arquivo)) {
            self->eventos_ignorados.remove(nome_arquivo);
            return;
        }

        if (acao == 3 && QFileInfo(caminho_completo).isDir()) return;

        const QString ext = QStringLiteral(".") + QFileInfo(nome_arquivo).suffix().toLower();
        const QSet<QString> especiais = {".heic", ".xlsx", ".xls", ".docx", ".pdf", ".mp4", ".mov", ".avi", ".mkv", ".xml"};
        const double tempo_limite_especial = 0.1;
        const double tempo_limite_normal = 0.05;

        if (acao == 2) {
            if (self->arquivos_recem_excluidos.contains(nome_arquivo)) {
                const double tempo_limite = especiais.contains(ext) ? tempo_limite_especial : tempo_limite_normal;
                if ((tempo_atual - self->arquivos_recem_excluidos.value(nome_arquivo)) < tempo_limite) return;
            }
            self->evento_excluido->processar(nome_arquivo, caminho_completo, tempo_atual);
            return;
        }

        if (acao == 4 || acao == 5) {
            std::lock_guard<std::mutex> lock(self->registros_lock);
            self->evento_renomeado->processar(nome_arquivo, caminho_completo, acao);
            return;
        }

        if (acao == 1) {
            if (self->arquivos_recem_adicionados.contains(nome_arquivo)) {
                const double tempo_limite = especiais.contains(ext) ? tempo_limite_especial : tempo_limite_normal;
                if ((tempo_atual - self->arquivos_recem_adicionados.value(nome_arquivo)) < tempo_limite) return;
            }
            self->arquivos_recem_adicionados[nome_arquivo] = tempo_atual;
            self->evento_adicionado->processar(nome_arquivo, caminho_completo, tempo_atual);
            return;
        }

        if (acao == 3) {
            if (self->arquivos_recem_modificados.contains(nome_arquivo)) {
                const double tempo_limite = especiais.contains(ext) ? tempo_limite_especial : tempo_limite_normal;
                if ((tempo_atual - self->arquivos_recem_modificados.value(nome_arquivo)) < tempo_limite) return;
            }
            if (QFileInfo::exists(caminho_completo)) {
                self->evento_modificado->processar(nome_arquivo, caminho_completo, tempo_atual);
            }
        }

        self->falhas_consecutivas = 0;
    } catch (...) {
        if (self) {
            self->falhas_consecutivas += 1;
            self->ultimo_erro = QStringLiteral("Erro ao processar evento");
            source::utils::LogManager::get_logger().error("Erro ao processar evento", true);
        }
    }
}

}  // namespace source::services::GerenciamentoObservador
