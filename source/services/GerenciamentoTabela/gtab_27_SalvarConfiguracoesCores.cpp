#include "gtab_27_SalvarConfiguracoesCores.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/CaminhoPersistenteUtils.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void salvar_configuracoes_cores(source::services::GerenciadorTabela* self) {
    try {
        if (!self) return;
        const QString config_path = QDir(source::utils::obter_caminho_persistente()).filePath(QStringLiteral("colunas_coloridas.json"));
        QDir().mkpath(QFileInfo(config_path).absolutePath());

        QJsonArray arr;
        for (const QString& key : self->colunas_para_colorir) arr.append(key);
        QJsonObject obj;
        obj.insert(QStringLiteral("colunas_para_colorir"), arr);

        QFile f(config_path);
        if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            f.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
            f.close();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em salvar_configuracoes_cores", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

