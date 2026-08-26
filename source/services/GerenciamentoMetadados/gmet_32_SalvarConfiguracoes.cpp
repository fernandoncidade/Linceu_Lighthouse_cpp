#include "gmet_32_SalvarConfiguracoes.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

void salvar_configuracoes(source::services::GerenciadorColunas* gc) {
    try {
        if (!gc) return;
        QJsonObject config;
        for (auto it = gc->COLUNAS_DISPONIVEIS.constBegin(); it != gc->COLUNAS_DISPONIVEIS.constEnd(); ++it) {
            QJsonObject o;
            o.insert("visivel", it.value().value("visivel").toBool());
            o.insert("ordem", it.value().value("ordem").toInt());
            config.insert(it.key(), o);
        }
        QFile f(gc->config_path);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(QJsonDocument(config).toJson(QJsonDocument::Indented));
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao salvar configurações", true);
    }
}

}  // namespace source::services::GerenciamentoMetadados
