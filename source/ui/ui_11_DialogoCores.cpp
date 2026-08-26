#include "ui_11_DialogoCores.h"

#include <QApplication>
#include <QColorDialog>
#include <QMetaObject>
#include <QString>
#include <QVariant>
#include <QWidget>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

#include "source/ui/GerenciamentoDialogoCores/gdc_01_setup_ui.h"
#include "source/ui/GerenciamentoDialogoCores/gdc_02_adicionar_grid_cores.h"
#include "source/ui/GerenciamentoDialogoCores/gdc_03_selecionar_cor.h"
#include "source/ui/GerenciamentoDialogoCores/gdc_04_abrir_seletor_avancado.h"
#include "source/ui/GerenciamentoDialogoCores/gdc_05_accept.h"
#include "source/ui/GerenciamentoDialogoCores/gdc_06_traduzir_dialogo_cores.h"
#include "source/ui/GerenciamentoDialogoCores/gdc_07_obter_cor.h"
#include "source/ui/GerenciamentoDialogoCores/gdc_08_atualizar_traducoes.h"
#include "source/ui/ui_12_LocalizadorQt.h"

#include "source/utils/LogManager.h"

namespace source::ui {
namespace {

QWidget* obter_interface_principal(const source::ui::DialogoPaletaCores* self) {
    if (self == nullptr) {
        return nullptr;
    }

    QObject* interface_obj = self->property("interface_principal").value<QObject*>();
    auto* interface_widget = qobject_cast<QWidget*>(interface_obj);
    return interface_widget != nullptr ? interface_widget->window() : nullptr;
}

QWidget* obter_painel_filtros(const source::ui::DialogoPaletaCores* self) {
    QWidget* interface_principal = obter_interface_principal(self);
    if (interface_principal == nullptr) {
        return nullptr;
    }

    QObject* filtros_obj = interface_principal->property("painel_filtros").value<QObject*>();
    auto* filtros_widget = qobject_cast<QWidget*>(filtros_obj);
    return filtros_widget != nullptr ? filtros_widget->window() : nullptr;
}

QWidget* obter_dialogo_avancado(const source::ui::DialogoPaletaCores* self) {
    if (self == nullptr) {
        return nullptr;
    }

    QObject* dialogo_obj = self->property("_dialogo_cor_avancado").value<QObject*>();
    auto* dialogo_widget = qobject_cast<QWidget*>(dialogo_obj);
    return dialogo_widget != nullptr ? dialogo_widget->window() : nullptr;
}

void atualizar_event_filter_interface_principal(source::ui::DialogoPaletaCores* self) {
    if (self == nullptr) {
        return;
    }

    QObject* atual_obj = self->property("_interface_principal_event_filter").value<QObject*>();
    auto* atual = qobject_cast<QWidget*>(atual_obj);
    QWidget* desejado = obter_interface_principal(self);

    if (atual == desejado) {
        return;
    }

    if (atual != nullptr) {
        atual->removeEventFilter(self);
    }
    if (desejado != nullptr) {
        desejado->installEventFilter(self);
    }

    self->setProperty("_interface_principal_event_filter",
                      QVariant::fromValue(static_cast<QObject*>(desejado)));
}

void atualizar_event_filter_painel_filtros(source::ui::DialogoPaletaCores* self) {
    if (self == nullptr) {
        return;
    }

    QObject* atual_obj = self->property("_painel_filtros_event_filter").value<QObject*>();
    auto* atual = qobject_cast<QWidget*>(atual_obj);
    QWidget* desejado = obter_painel_filtros(self);

    if (atual == desejado) {
        return;
    }

    if (atual != nullptr) {
        atual->removeEventFilter(self);
    }
    if (desejado != nullptr) {
        desejado->installEventFilter(self);
    }

    self->setProperty("_painel_filtros_event_filter",
                      QVariant::fromValue(static_cast<QObject*>(desejado)));
}

QString obter_id_janela_gerenciada(const source::ui::DialogoPaletaCores* self, QWidget* janela) {
    if (self == nullptr || janela == nullptr) {
        return QString();
    }

    if (janela == obter_interface_principal(self)) {
        return QStringLiteral("gui");
    }
    if (janela == self) {
        return QStringLiteral("palette");
    }
    if (janela == obter_dialogo_avancado(self)) {
        return QStringLiteral("advanced");
    }
    return QString();
}

QWidget* obter_janela_por_id(source::ui::DialogoPaletaCores* self, const QString& id) {
    if (self == nullptr) {
        return nullptr;
    }

    if (id == QStringLiteral("gui")) {
        return obter_interface_principal(self);
    }
    if (id == QStringLiteral("palette")) {
        return self;
    }
    if (id == QStringLiteral("advanced")) {
        return obter_dialogo_avancado(self);
    }
    return nullptr;
}

QStringList sincronizar_ordem_janelas_gerenciadas(source::ui::DialogoPaletaCores* self) {
    if (self == nullptr) {
        return {};
    }

    QStringList disponiveis;
    if (QWidget* interface_principal = obter_interface_principal(self);
        interface_principal != nullptr && interface_principal->isVisible()) {
        disponiveis << QStringLiteral("gui");
    }
    if (self->isVisible()) {
        disponiveis << QStringLiteral("palette");
    }
    if (QWidget* dialogo_avancado = obter_dialogo_avancado(self);
        dialogo_avancado != nullptr && dialogo_avancado->isVisible()) {
        disponiveis << QStringLiteral("advanced");
    }

    QStringList ordem = self->property("_hierarquia_janelas").toStringList();
    QStringList sincronizada;

    for (const QString& id : ordem) {
        if (disponiveis.contains(id) && !sincronizada.contains(id)) {
            sincronizada << id;
        }
    }
    for (const QString& id : disponiveis) {
        if (!sincronizada.contains(id)) {
            sincronizada << id;
        }
    }

    self->setProperty("_hierarquia_janelas", sincronizada);
    return sincronizada;
}

QList<QWidget*> obter_janelas_gerenciadas_ordenadas(source::ui::DialogoPaletaCores* self) {
    QList<QWidget*> janelas;
    if (self == nullptr) {
        return janelas;
    }

    const QStringList ordem = sincronizar_ordem_janelas_gerenciadas(self);
    for (const QString& id : ordem) {
        QWidget* janela = obter_janela_por_id(self, id);
        if (janela != nullptr && janela->isVisible()) {
            janelas << janela;
        }
    }

    return janelas;
}

void promover_janela_gerenciada(source::ui::DialogoPaletaCores* self, QWidget* janela_ativa) {
    if (self == nullptr || janela_ativa == nullptr) {
        return;
    }

    const QString id_ativo = obter_id_janela_gerenciada(self, janela_ativa);
    if (id_ativo.isEmpty()) {
        return;
    }

    QStringList ordem = sincronizar_ordem_janelas_gerenciadas(self);
    ordem.removeAll(id_ativo);
    ordem.prepend(id_ativo);
    self->setProperty("_hierarquia_janelas", ordem);
}

void colocar_em_segundo_plano(QWidget* janela, QWidget* referencia) {
    if (janela == nullptr || referencia == nullptr || janela == referencia) {
        return;
    }

#ifdef Q_OS_WIN
    janela->createWinId();
    referencia->createWinId();
    ::SetWindowPos(reinterpret_cast<HWND>(janela->winId()),
                   reinterpret_cast<HWND>(referencia->winId()),
                   0,
                   0,
                   0,
                   0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#else
    Q_UNUSED(referencia)
    janela->lower();
#endif
}

void trazer_para_frente(QWidget* janela) {
    if (janela == nullptr || !janela->isVisible()) {
        return;
    }

    janela->raise();
    janela->activateWindow();
}

void aplicar_hierarquia_janelas_gerenciadas(source::ui::DialogoPaletaCores* self) {
    if (self == nullptr || self->property("_atualizando_hierarquia_janelas").toBool()) {
        return;
    }

    self->setProperty("_atualizando_hierarquia_janelas", true);
    try {
        const QList<QWidget*> janelas = obter_janelas_gerenciadas_ordenadas(self);
        if (!janelas.isEmpty()) {
            trazer_para_frente(janelas.first());

            for (int i = 1; i < janelas.size(); ++i) {
                colocar_em_segundo_plano(janelas.at(i), janelas.at(i - 1));
            }

            if (QWidget* painel_filtros = obter_painel_filtros(self);
                painel_filtros != nullptr && painel_filtros->isVisible()) {
                colocar_em_segundo_plano(painel_filtros, janelas.last());
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            "Erro ao aplicar hierarquia das janelas de cor", true);
    }
    self->setProperty("_atualizando_hierarquia_janelas", false);
}

void aplicar_hierarquia_abaixo_de_referencia(source::ui::DialogoPaletaCores* self, QWidget* referencia_externa) {
    if (self == nullptr || referencia_externa == nullptr || self->property("_atualizando_hierarquia_janelas").toBool()) {
        return;
    }

    self->setProperty("_atualizando_hierarquia_janelas", true);
    try {
        const QList<QWidget*> janelas = obter_janelas_gerenciadas_ordenadas(self);
        QWidget* referencia_atual = referencia_externa;
        for (QWidget* janela : janelas) {
            if (janela == nullptr || !janela->isVisible()) {
                continue;
            }

            colocar_em_segundo_plano(janela, referencia_atual);
            referencia_atual = janela;
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            "Erro ao aplicar hierarquia secundária das janelas de cor", true);
    }
    self->setProperty("_atualizando_hierarquia_janelas", false);
}

}  // namespace

DialogoPaletaCores::DialogoPaletaCores(const QColor& cor_atual, QObject* interface_principal,
                                       const QString& titulo, QWidget* parent,
                                       const QString& tipo_operacao)
    : QDialog(parent),
      interface_(interface_principal),
      loc_(nullptr),
      tipo_operacao_(tipo_operacao),
      cor_atual_(cor_atual),
    cor_selecionada_(QColor()),
    preview_atual_(nullptr),
    preview_nova_(nullptr),
    label_atual_(nullptr),
    label_novo_(nullptr),
    tab_widget_(nullptr),
    btn_avancado_(nullptr),
    btn_ok_(nullptr),
    btn_cancelar_(nullptr),
    dialogo_avancado_(nullptr),
    qt_translator_(nullptr) {
    try {
        setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
        setModal(false);
        setAttribute(Qt::WA_DeleteOnClose, true);
        setProperty("interface_principal",
                    QVariant::fromValue(static_cast<QObject*>(interface_principal)));
        installEventFilter(this);

        if (interface_ != nullptr) {
            loc_ = interface_->property("loc").value<QObject*>();
        }

        if (loc_ == nullptr) {
            loc_ = new LocalizadorQt(this);
        }

        QString titulo_final = titulo;
        if (titulo_final.isEmpty()) {
            titulo_final = get_text(QStringLiteral("select_color"));
            if (titulo_final.isEmpty()) {
                titulo_final = QStringLiteral("select_color");
            }
        }
        setWindowTitle(titulo_final);
        setup_ui();

        if (loc_ != nullptr) {
            QObject::connect(loc_, SIGNAL(traducoes_carregadas(QString)), this, SLOT(atualizar_traducoes()));
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao configurar diálogo de cores", true);
    }
}

QObject* DialogoPaletaCores::loc_obj() const {
    return loc_;
}

QString DialogoPaletaCores::get_text(const QString& key) const {
    QString text;
    if (loc_ != nullptr) {
        QMetaObject::invokeMethod(loc_, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key));
    }
    return text;
}

void DialogoPaletaCores::setup_ui() { GerenciamentoDialogoCores::setup_ui(this); }
void DialogoPaletaCores::adicionar_grid_cores(QGridLayout* grid_layout, const QStringList& lista_cores, int colunas) {
    GerenciamentoDialogoCores::adicionar_grid_cores(this, grid_layout, lista_cores, colunas);
}
void DialogoPaletaCores::selecionar_cor(const QString& cor_hex) { GerenciamentoDialogoCores::selecionar_cor(this, cor_hex); }
void DialogoPaletaCores::abrir_seletor_avancado() { GerenciamentoDialogoCores::abrir_seletor_avancado(this); }
void DialogoPaletaCores::traduzir_dialogo_cores(QWidget* dialogo) { GerenciamentoDialogoCores::traduzir_dialogo_cores(this, dialogo); }
QColor DialogoPaletaCores::obter_cor() const { return GerenciamentoDialogoCores::obter_cor(const_cast<DialogoPaletaCores*>(this)); }
void DialogoPaletaCores::atualizar_traducoes() { GerenciamentoDialogoCores::atualizar_traducoes(this); }
void DialogoPaletaCores::accept() { GerenciamentoDialogoCores::aceitar(this); }

bool DialogoPaletaCores::eventFilter(QObject* watched, QEvent* event) {
    try {
        if (event != nullptr && event->type() == QEvent::WindowActivate) {
            if (watched == obter_painel_filtros(this)) {
                aplicar_hierarquia_abaixo_de_referencia(this, qobject_cast<QWidget*>(watched));
            } else if (QWidget* janela_ativada = qobject_cast<QWidget*>(watched);
                       !this->property("_atualizando_hierarquia_janelas").toBool()) {
                promover_janela_gerenciada(this, janela_ativada);
                aplicar_hierarquia_janelas_gerenciadas(this);
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            "Erro ao ajustar z-order do dialogo de cores", true);
    }

    return QDialog::eventFilter(watched, event);
}

void DialogoPaletaCores::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);

    try {
        atualizar_event_filter_interface_principal(this);
        atualizar_event_filter_painel_filtros(this);
        promover_janela_gerenciada(this, this);
        aplicar_hierarquia_janelas_gerenciadas(this);
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            "Erro ao exibir dialogo de cores", true);
    }
}

void DialogoPaletaCores::closeEvent(QCloseEvent* event) {
    try {
        if (dialogo_avancado_ != nullptr) {
            dialogo_avancado_->close();
        }

        QWidget* painel_filtros = obter_painel_filtros(this);
        if (painel_filtros != nullptr && painel_filtros->isVisible()) {
            trazer_para_frente(painel_filtros);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            "Erro ao fechar dialogo de cores", true);
    }

    QDialog::closeEvent(event);
}

}  // namespace source::ui
