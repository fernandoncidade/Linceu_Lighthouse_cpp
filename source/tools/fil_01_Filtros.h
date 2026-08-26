#pragma once

#include <memory>

#include <QCloseEvent>
#include <QEvent>
#include <QShowEvent>
#include <QWidget>

namespace source::ui {
class GerenciadorBotoesUI;
class LocalizadorQt;
}

namespace source::tools {

class AdministradorCalendario;
class AdministradorFiltros;

class Filtros : public QWidget {
    Q_OBJECT

public:
    explicit Filtros(QWidget* tabela_dados, source::ui::LocalizadorQt* loc = nullptr, QWidget* parent = nullptr);
    ~Filtros() override;

    source::ui::LocalizadorQt* localizador() const;
    QWidget* tabela_dados() const;
    source::ui::GerenciadorBotoesUI* gerenciador_botoes_obj() const;
    source::tools::AdministradorCalendario* administrador_calendario_obj() const;
    source::tools::AdministradorFiltros* administrador_filtros_obj() const;
    void set_tabela_dados(QWidget* tabela_dados);

    void on_idioma_alterado(const QString& novo_idioma = QString());
    void desconectar_sinais();
    void atualizar_status(const QString& idioma = QString());
    void setup_ui();
    void on_filtro_alterado();
    void sincronizar_com_menu_principal();
    Q_INVOKABLE bool verificar_filtro_operacao(const QString& operacao) const;
    QString atualizar_contagem();
    void atualizar_interface(const QString& idioma = QString());
    void limpar_filtros();

signals:
    void filtroAplicado();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    source::ui::LocalizadorQt* loc_;
    QWidget* tabela_dados_;
    std::unique_ptr<source::ui::GerenciadorBotoesUI> gerenciador_botoes_;
    std::unique_ptr<source::tools::AdministradorCalendario> administrador_calendario_;
    std::unique_ptr<source::tools::AdministradorFiltros> administrador_filtros_;
};

}  // namespace source::tools
