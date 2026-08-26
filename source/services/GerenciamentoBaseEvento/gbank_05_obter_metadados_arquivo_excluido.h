#pragma once

#include <QString>
#include <QVariantMap>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
QVariantMap obter_metadados_arquivo_excluido(source::services::BaseEvento* self, const QString& nome, const QString& caminho);
}
