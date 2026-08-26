#include "gmet_31_CarregarConfiguracoes.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

void carregar_configuracoes(source::services::GerenciadorColunas* gc) {
    try {
        if (!gc) return;
        QFile f(gc->config_path);
        if (!f.exists() || !f.open(QIODevice::ReadOnly)) return;
        const QJsonObject obj = QJsonDocument::fromJson(f.readAll()).object();
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            if (!gc->COLUNAS_DISPONIVEIS.contains(it.key())) continue;
            QVariantMap c = gc->COLUNAS_DISPONIVEIS.value(it.key());
            const QJsonObject v = it.value().toObject();
            c["visivel"] = v.value("visivel").toBool(c.value("visivel").toBool());
            c["ordem"] = v.value("ordem").toInt(c.value("ordem").toInt());
            gc->COLUNAS_DISPONIVEIS[it.key()] = c;
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao carregar configurações", true);
    }
}

}  // namespace source::services::GerenciamentoMetadados
