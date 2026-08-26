#include "ob_07_EventoRenomeado.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>

#include <algorithm>

#include "ob_01_Observador.h"
#include "source/ui/ui_12_LocalizadorQt.h"

namespace source::services {
namespace {

QString normalizar_diretorio(const QString& caminho) {
    if (caminho.trimmed().isEmpty()) {
        return {};
    }
    return QDir::toNativeSeparators(QDir::cleanPath(QFileInfo(caminho).absolutePath())).toLower();
}

bool mesmo_diretorio(const QString& lhs, const QString& rhs) {
    const QString left = normalizar_diretorio(lhs);
    const QString right = normalizar_diretorio(rhs);
    return !left.isEmpty() && left == right;
}

bool mudou_de_diretorio(const QString& dir_anterior, const QString& dir_atual) {
    const QString origem = normalizar_diretorio(dir_anterior);
    const QString destino = normalizar_diretorio(dir_atual);
    return !origem.isEmpty() && !destino.isEmpty() && origem != destino;
}

std::deque<std::pair<QString, QString>>::iterator localizar_registro_antigo(
    std::deque<std::pair<QString, QString>>& pendentes,
    const QString& nome_novo,
    const QString& caminho_novo) {
    if (pendentes.empty()) {
        return pendentes.end();
    }

    auto it = std::find_if(pendentes.begin(), pendentes.end(), [&caminho_novo](const auto& item) {
        return mesmo_diretorio(item.second, caminho_novo);
    });
    if (it != pendentes.end()) {
        return it;
    }

    const QString base_nova = QFileInfo(nome_novo).fileName();
    it = std::find_if(pendentes.begin(), pendentes.end(), [&base_nova](const auto& item) {
        return QFileInfo(item.first).fileName().compare(base_nova, Qt::CaseInsensitive) == 0;
    });
    if (it != pendentes.end()) {
        return it;
    }

    return pendentes.begin();
}

}  // namespace

EventoRenomeado::EventoRenomeado(source::services::Observador* observador)
    : BaseEvento(observador) {}

bool EventoRenomeado::processar(const QString& nome_arquivo, const QString& caminho_completo, int acao) {
    if (!observador) return true;

    const double tempo_atual = QDateTime::currentMSecsSinceEpoch() / 1000.0;

    if (acao == 4) {
        observador->registros_anteriores[caminho_completo] = nome_arquivo;
        observador->registros_anteriores[nome_arquivo] = caminho_completo;
        observador->renomeacoes_pendentes.push_back({nome_arquivo, caminho_completo});
        while (observador->renomeacoes_pendentes.size() > 8192) {
            observador->renomeacoes_pendentes.pop_front();
        }
        return true;
    }

    if (acao == 5) {
        const QString dir_atual = caminho_completo;
        const QString dir_base = QFileInfo(dir_atual).absolutePath();

        QString nome_antigo;
        QString dir_anterior;

        auto it_pendente = localizar_registro_antigo(observador->renomeacoes_pendentes, nome_arquivo, caminho_completo);
        if (it_pendente != observador->renomeacoes_pendentes.end()) {
            nome_antigo = it_pendente->first;
            dir_anterior = it_pendente->second;
            observador->renomeacoes_pendentes.erase(it_pendente);
        }

        if (nome_antigo.isEmpty() || dir_anterior.isEmpty()) {
            const QString caminho_mesmo_dir = QDir(dir_base).filePath(nome_arquivo);
            if (observador->registros_anteriores.contains(caminho_mesmo_dir)) {
                dir_anterior = caminho_mesmo_dir;
                nome_antigo = observador->registros_anteriores.value(caminho_mesmo_dir);
            } else if (observador->registros_anteriores.contains(nome_arquivo)) {
                nome_antigo = nome_arquivo;
                dir_anterior = observador->registros_anteriores.value(nome_arquivo);
            }
        }

        if (!nome_antigo.isEmpty() && !dir_anterior.isEmpty()) {
            observador->registros_anteriores.remove(dir_anterior);
            observador->registros_anteriores.remove(nome_antigo);
            observador->arquivos_recem_renomeados[nome_arquivo] = tempo_atual;
            observador->arquivos_recem_adicionados[nome_arquivo] = tempo_atual;
            observador->ultima_modificacao[nome_arquivo] = tempo_atual;

            const QString operacao = mudou_de_diretorio(dir_anterior, caminho_completo)
                                         ? QStringLiteral("op_moved")
                                         : QStringLiteral("op_renamed");
            notificar_evento(operacao, nome_arquivo, dir_anterior, caminho_completo);
            return true;
        }
    }

    return true;
}

}  // namespace source::services
