#pragma once

#include <QString>
#include <QStringList>
#include <QVariantMap>

class QObject;

namespace source::ui {

class GerenciadorMonitoramento {
public:
    explicit GerenciadorMonitoramento(QObject* interface_principal);

    void reiniciar_sistema_monitoramento();
    bool adicionar_diretorio_monitorado(const QString& diretorio);
    bool remover_diretorio_monitorado(const QString& diretorio);
    QStringList obter_diretorios_monitorados() const;
    QVariantMap obter_resumo_seguranca() const;
    QString exportar_pacote_evidencia(const QString& destino = QString()) const;

private:
    QObject* interface_;
};

}  // namespace source::ui
