#pragma once

#include <QDateTime>

class QObject;

namespace source::ui {

class GerenciadorProgressoUI {
public:
    explicit GerenciadorProgressoUI(QObject* interface_principal);

    void criar_barra_progresso();
    void atualizar_progresso_scan(int progresso, int contador, int total);
    void atualizar_progresso_traducao(int progresso, int contador, int total);

private:
    QObject* interface_;
    QObject* loc_;
    QDateTime scan_start_time_;
    bool has_scan_start_time_;
};

}  // namespace source::ui
