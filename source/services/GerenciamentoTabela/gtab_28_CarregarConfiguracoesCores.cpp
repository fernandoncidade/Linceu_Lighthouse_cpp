#include "gtab_28_CarregarConfiguracoesCores.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/CaminhoPersistenteUtils.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void carregar_configuracoes_cores(source::services::GerenciadorTabela* self) {
    try {
        if (!self) return;

        const QString config_path = QDir(source::utils::obter_caminho_persistente()).filePath(QStringLiteral("colunas_coloridas.json"));
        if (QFile::exists(config_path)) {
            QFile f(config_path);
            if (f.open(QIODevice::ReadOnly)) {
                const QJsonObject obj = QJsonDocument::fromJson(f.readAll()).object();
                const QJsonArray arr = obj.value(QStringLiteral("colunas_para_colorir")).toArray();
                self->colunas_para_colorir.clear();
                for (const QJsonValue& v : arr) self->colunas_para_colorir.insert(v.toString());
            }
        }

        self->_invalidar_cache_cores();
        self->atualizar_cores_colunas(true);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em carregar_configuracoes_cores", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
