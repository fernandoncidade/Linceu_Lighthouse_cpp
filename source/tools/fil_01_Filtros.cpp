#include "fil_01_Filtros.h"

#include <QApplication>
#include <QGuiApplication>
#include <QIcon>
#include <QScreen>
#include <QWindow>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

#include "fil_02_AdministradorCalendario.h"
#include "fil_03_AdministradorFiltros.h"
#include "source/tools/GerenciamentoFiltros/gfil_01_on_idioma_alterado.h"
#include "source/tools/GerenciamentoFiltros/gfil_02_desconectar_sinais.h"
#include "source/tools/GerenciamentoFiltros/gfil_03_atualizar_status.h"
#include "source/tools/GerenciamentoFiltros/gfil_04_setup_ui.h"
#include "source/tools/GerenciamentoFiltros/gfil_05_on_filtro_alterado.h"
#include "source/tools/GerenciamentoFiltros/gfil_06_sincronizar_com_menu_principal.h"
#include "source/tools/GerenciamentoFiltros/gfil_07_verificar_filtro_operacao.h"
#include "source/tools/GerenciamentoFiltros/gfil_08_atualizar_contagem.h"
#include "source/tools/GerenciamentoFiltros/gfil_09_atualizar_interface.h"
#include "source/tools/GerenciamentoFiltros/gfil_10_limpar_filtros.h"
#include "source/ui/ui_02_GerenciadorBotoesUI.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/IconUtils.h"
#include "source/utils/LogManager.h"

namespace source::tools {
namespace {

QWidget* obter_janela_principal(const source::tools::Filtros* self) {
    if (self == nullptr) {
        return nullptr;
    }

    QObject* janela_obj = self->property("janela_principal").value<QObject*>();
    auto* janela_widget = qobject_cast<QWidget*>(janela_obj);
    return janela_widget != nullptr ? janela_widget->window() : nullptr;
}

void atualizar_event_filter_janela_principal(source::tools::Filtros* self) {
    if (self == nullptr) {
        return;
    }

    QObject* atual_obj = self->property("_janela_principal_event_filter").value<QObject*>();
    auto* atual = qobject_cast<QWidget*>(atual_obj);
    QWidget* desejada = obter_janela_principal(self);

    if (atual == desejada) {
        return;
    }

    if (atual != nullptr) {
        atual->removeEventFilter(self);
    }
    if (desejada != nullptr) {
        desejada->installEventFilter(self);
    }

    self->setProperty("_janela_principal_event_filter",
                      QVariant::fromValue(static_cast<QObject*>(desejada)));
}

QScreen* obter_tela_referencia(QWidget* referencia) {
    if (referencia != nullptr) {
        if (QWindow* handle = referencia->windowHandle(); handle != nullptr && handle->screen() != nullptr) {
            return handle->screen();
        }
        if (QScreen* screen = referencia->screen(); screen != nullptr) {
            return screen;
        }
        if (QScreen* screen = QGuiApplication::screenAt(referencia->frameGeometry().center()); screen != nullptr) {
            return screen;
        }
    }

    return QApplication::primaryScreen();
}

void centralizar_no_monitor_referencia(QWidget* janela, QWidget* referencia) {
    if (janela == nullptr) {
        return;
    }

    QScreen* screen = obter_tela_referencia(referencia);
    if (screen == nullptr) {
        return;
    }

    const QRect available = screen->availableGeometry();
    QSize tamanho = janela->frameGeometry().size();
    if (!tamanho.isValid() || tamanho.isEmpty()) {
        tamanho = janela->sizeHint().expandedTo(janela->minimumSizeHint());
    }

    const int x = available.x() + qMax(0, (available.width() - tamanho.width()) / 2);
    const int y = available.y() + qMax(0, (available.height() - tamanho.height()) / 2);
    janela->move(x, y);
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

}  // namespace

Filtros::Filtros(QWidget* tabela_dados, source::ui::LocalizadorQt* loc, QWidget* parent)
    : QWidget(nullptr),
      loc_(loc != nullptr ? loc : new source::ui::LocalizadorQt(this)),
      tabela_dados_(tabela_dados),
      gerenciador_botoes_(std::make_unique<source::ui::GerenciadorBotoesUI>(this)),
      administrador_calendario_(std::make_unique<source::tools::AdministradorCalendario>(this)),
      administrador_filtros_(std::make_unique<source::tools::AdministradorFiltros>(this)) {
    try {
        setProperty("janela_principal", QVariant::fromValue(static_cast<QObject*>(parent)));
        setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
        setAttribute(Qt::WA_DeleteOnClose, false);
        setWindowModality(Qt::NonModal);
        installEventFilter(this);

        if (loc_ != nullptr) {
            setWindowTitle(loc_->get_text(QStringLiteral("advanced_filters")));
            QObject::connect(loc_, &source::ui::LocalizadorQt::idioma_alterado, this,
                             &source::tools::Filtros::on_idioma_alterado);
        }

        setWindowIcon(QIcon(source::utils::get_icon_path(QStringLiteral("filtro.ico"))));

        QObject::connect(this, &QObject::destroyed, this, [this]() { desconectar_sinais(); });

        setup_ui();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar filtros", true);
    }
}

Filtros::~Filtros() {
    removeEventFilter(this);
    if (auto* janela_principal = qobject_cast<QWidget*>(
            property("_janela_principal_event_filter").value<QObject*>())) {
        janela_principal->removeEventFilter(this);
    }
}

source::ui::LocalizadorQt* Filtros::localizador() const { return loc_; }
QWidget* Filtros::tabela_dados() const { return tabela_dados_; }
source::ui::GerenciadorBotoesUI* Filtros::gerenciador_botoes_obj() const { return gerenciador_botoes_.get(); }
source::tools::AdministradorCalendario* Filtros::administrador_calendario_obj() const { return administrador_calendario_.get(); }
source::tools::AdministradorFiltros* Filtros::administrador_filtros_obj() const { return administrador_filtros_.get(); }
void Filtros::set_tabela_dados(QWidget* tabela_dados) { tabela_dados_ = tabela_dados; }

void Filtros::on_idioma_alterado(const QString& novo_idioma) { GerenciamentoFiltros::on_idioma_alterado(this, novo_idioma); }
void Filtros::desconectar_sinais() { GerenciamentoFiltros::desconectar_sinais(this); }
void Filtros::atualizar_status(const QString& idioma) { GerenciamentoFiltros::atualizar_status(this, idioma); }
void Filtros::setup_ui() { GerenciamentoFiltros::setup_ui(this); }
void Filtros::on_filtro_alterado() { GerenciamentoFiltros::on_filtro_alterado(this); }
void Filtros::sincronizar_com_menu_principal() { GerenciamentoFiltros::sincronizar_com_menu_principal(this); }
bool Filtros::verificar_filtro_operacao(const QString& operacao) const { return GerenciamentoFiltros::verificar_filtro_operacao(const_cast<Filtros*>(this), operacao); }
QString Filtros::atualizar_contagem() { return GerenciamentoFiltros::atualizar_contagem(this); }
void Filtros::atualizar_interface(const QString& idioma) { GerenciamentoFiltros::atualizar_interface(this, idioma); }
void Filtros::limpar_filtros() { GerenciamentoFiltros::limpar_filtros(this); }

bool Filtros::eventFilter(QObject* watched, QEvent* event) {
    try {
        QWidget* janela_principal = obter_janela_principal(this);
        if (event != nullptr && event->type() == QEvent::WindowActivate) {
            if (watched == janela_principal) {
                if (isVisible()) {
                    colocar_em_segundo_plano(this, janela_principal);
                }
            } else if (watched == this) {
                if (janela_principal != nullptr && janela_principal->isVisible()) {
                    colocar_em_segundo_plano(janela_principal, this);
                }
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao ajustar z-order da janela de filtros", true);
    }

    return QWidget::eventFilter(watched, event);
}

void Filtros::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);

    try {
        atualizar_event_filter_janela_principal(this);
        centralizar_no_monitor_referencia(this, obter_janela_principal(this));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao exibir janela de filtros", true);
    }
}

void Filtros::closeEvent(QCloseEvent* event) {
    try {
        QWidget* janela_principal = obter_janela_principal(this);
        if (janela_principal != nullptr &&
            janela_principal->isVisible() &&
            !janela_principal->property("_encerrando_interface").toBool()) {
            janela_principal->raise();
            janela_principal->activateWindow();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao fechar janela de filtros", true);
    }

    QWidget::closeEvent(event);
}

}  // namespace source::tools
