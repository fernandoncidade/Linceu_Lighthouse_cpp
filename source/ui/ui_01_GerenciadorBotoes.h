#pragma once

#include <QString>

class QObject;

namespace source::ui {

class GerenciadorBotoes {
public:
    GerenciadorBotoes(QObject* interface_obj, QObject* loc_obj);

    QObject* interface_obj() const;
    QObject* loc_obj() const;

    void selecionar_diretorio();
    void alternar_analise_diretorio();
    bool exportar_dados(void* tabela_dados, const QString& formato, const QString& destino);
    bool exportar_para_sqlite(void* tabela_dados, const QString& destino);
    void limpar_dados();
    void pausar_monitoramento_ou_escaneamento();

private:
    QObject* interface_;
    QObject* loc_;
};

}  // namespace source::ui
