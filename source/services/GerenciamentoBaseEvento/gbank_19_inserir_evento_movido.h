#pragma once

#include <QVariantMap>

class QSqlQuery;
namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
QVariantMap preparar_evento_movido(source::services::BaseEvento* self, const QVariantMap& evento);
bool _inserir_evento_movido(source::services::BaseEvento* self, const QVariantMap& evento);
bool _inserir_evento_movido_query(source::services::BaseEvento* self, QSqlQuery& query, const QVariantMap& evento);
}
