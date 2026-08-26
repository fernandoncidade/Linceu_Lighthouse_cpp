#pragma once

#include <QColor>
#include <QCloseEvent>
#include <QDialog>
#include <QEvent>
#include <QShowEvent>
#include <QStringList>

class QObject;
class QFrame;
class QLabel;
class QPushButton;
class QTabWidget;
class QGridLayout;
class QWidget;
class QColorDialog;
class QTranslator;

namespace source::ui {
class DialogoPaletaCores;
}

namespace source::ui::GerenciamentoDialogoCores {
void setup_ui(source::ui::DialogoPaletaCores* self);
void adicionar_grid_cores(source::ui::DialogoPaletaCores* self, QGridLayout* grid_layout,
                          const QStringList& lista_cores, int colunas);
void selecionar_cor(source::ui::DialogoPaletaCores* self, const QString& cor_hex);
void abrir_seletor_avancado(source::ui::DialogoPaletaCores* self);
void aceitar(source::ui::DialogoPaletaCores* self);
void traduzir_dialogo_cores(source::ui::DialogoPaletaCores* self, QWidget* dialogo = nullptr);
QColor obter_cor(source::ui::DialogoPaletaCores* self);
void atualizar_traducoes(source::ui::DialogoPaletaCores* self);
}

namespace source::ui {

class DialogoPaletaCores : public QDialog {
    Q_OBJECT

public:
    explicit DialogoPaletaCores(const QColor& cor_atual, QObject* interface_principal,
                                const QString& titulo = QString(), QWidget* parent = nullptr,
                                const QString& tipo_operacao = QString());

    QObject* loc_obj() const;
    QString get_text(const QString& key) const;
    QColor obter_cor() const;

public slots:
    void atualizar_traducoes();

signals:
    void corSelecionada(const QColor& cor);

protected:
    void accept() override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    void setup_ui();
    void adicionar_grid_cores(QGridLayout* grid_layout, const QStringList& lista_cores, int colunas);
    void selecionar_cor(const QString& cor_hex);
    void abrir_seletor_avancado();
    void traduzir_dialogo_cores(QWidget* dialogo = nullptr);

private:
    friend void source::ui::GerenciamentoDialogoCores::setup_ui(source::ui::DialogoPaletaCores* self);
    friend void source::ui::GerenciamentoDialogoCores::adicionar_grid_cores(source::ui::DialogoPaletaCores* self, QGridLayout* grid_layout,
                                                                             const QStringList& lista_cores, int colunas);
    friend void source::ui::GerenciamentoDialogoCores::selecionar_cor(source::ui::DialogoPaletaCores* self, const QString& cor_hex);
    friend void source::ui::GerenciamentoDialogoCores::abrir_seletor_avancado(source::ui::DialogoPaletaCores* self);
    friend void source::ui::GerenciamentoDialogoCores::aceitar(source::ui::DialogoPaletaCores* self);
    friend void source::ui::GerenciamentoDialogoCores::traduzir_dialogo_cores(source::ui::DialogoPaletaCores* self, QWidget* dialogo);
    friend QColor source::ui::GerenciamentoDialogoCores::obter_cor(source::ui::DialogoPaletaCores* self);
    friend void source::ui::GerenciamentoDialogoCores::atualizar_traducoes(source::ui::DialogoPaletaCores* self);

private:
    QObject* interface_;
    QObject* loc_;
    QString tipo_operacao_;
    QColor cor_atual_;
    QColor cor_selecionada_;
    QFrame* preview_atual_;
    QFrame* preview_nova_;
    QLabel* label_atual_;
    QLabel* label_novo_;
    QTabWidget* tab_widget_;
    QPushButton* btn_avancado_;
    QPushButton* btn_ok_;
    QPushButton* btn_cancelar_;
    QColorDialog* dialogo_avancado_;
    QTranslator* qt_translator_;
};

}  // namespace source::ui
