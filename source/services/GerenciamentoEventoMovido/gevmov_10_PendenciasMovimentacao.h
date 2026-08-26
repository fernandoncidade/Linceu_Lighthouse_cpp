#pragma once

#include <QHash>
#include <QList>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>

namespace source::services::GerenciamentoEventoMovido {

class PendenciasMovimentacao final : public QObject {
public:
    explicit PendenciasMovimentacao(QObject* parent = nullptr) : QObject(parent) {}

    void clear() {
        excluidos.clear();
        adicionados.clear();
    }

    QHash<QString, QList<QVariantMap>> excluidos;
    QHash<QString, QList<QVariantMap>> adicionados;
};

inline PendenciasMovimentacao* obter_pendencias_movimentacao(QObject* interfaceMonitor) {
    if (interfaceMonitor == nullptr) {
        return nullptr;
    }

    QObject* obj = interfaceMonitor->property("movimentacao_pendencias").value<QObject*>();
    auto* estado = dynamic_cast<PendenciasMovimentacao*>(obj);
    if (estado != nullptr) {
        return estado;
    }

    estado = new PendenciasMovimentacao(interfaceMonitor);
    interfaceMonitor->setProperty("movimentacao_pendencias", QVariant::fromValue(static_cast<QObject*>(estado)));
    interfaceMonitor->setProperty("excluidos_recentemente_obj", QVariant::fromValue(static_cast<QObject*>(estado)));
    return estado;
}

inline void limpar_pendencias_movimentacao(QObject* interfaceMonitor) {
    if (auto* estado = obter_pendencias_movimentacao(interfaceMonitor)) {
        estado->clear();
    }
    if (interfaceMonitor != nullptr) {
        interfaceMonitor->setProperty("excluidos_recentemente", QVariantMap());
        interfaceMonitor->setProperty("adicionados_recentemente", QVariantMap());
    }
}

}  // namespace source::services::GerenciamentoEventoMovido
