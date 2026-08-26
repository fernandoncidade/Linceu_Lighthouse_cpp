#pragma once

#include <functional>

#include <QList>
#include <QVariantMap>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
void processar_eventos_movimentacao(source::services::BaseEvento* self, const QList<QVariantMap>& eventos_movidos, const std::function<void()>& callback = {});
}
