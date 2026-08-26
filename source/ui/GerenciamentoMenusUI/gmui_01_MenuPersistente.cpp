#include "gmui_01_MenuPersistente.h"

#include <QAction>
#include <QCursor>
#include <QEvent>
#include <QHideEvent>
#include <QMouseEvent>
#include <QList>
#include <QPointer>
#include <QShowEvent>
#include <QTimer>

#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoMenusUI {
namespace {

constexpr int kMenuCloseDelayMs = 180;
constexpr int kMenuHoverTolerancePx = 24;

class MenuHoverTracker final : public QObject {
public:
    MenuHoverTracker() {
        monitor_timer_.setInterval(140);
        QObject::connect(&monitor_timer_, &QTimer::timeout, &monitor_timer_, [this]() {
            reevaluate_close_state();
        });

        close_timer_.setSingleShot(true);
        close_timer_.setInterval(kMenuCloseDelayMs);
        QObject::connect(&close_timer_, &QTimer::timeout, &close_timer_, [this]() {
            close_visible_menus_if_needed();
        });
    }

    void register_menu(MenuPersistente* menu) {
        if (menu == nullptr) {
            return;
        }
        prune_hidden_menus();
        const QPointer<MenuPersistente> pointer(menu);
        if (!visible_menus_.contains(pointer)) {
            visible_menus_.append(pointer);
        }
        ensure_monitoring();
        reevaluate_close_state();
    }

    void unregister_menu(MenuPersistente* menu) {
        if (menu == nullptr) {
            return;
        }
        prune_hidden_menus();
        visible_menus_.removeAll(QPointer<MenuPersistente>(menu));
        ensure_monitoring();
        reevaluate_close_state();
    }

    void note_cursor_activity() {
        if (visible_menus_.isEmpty()) {
            return;
        }
        if (!monitor_timer_.isActive()) {
            monitor_timer_.start();
        }
        reevaluate_close_state();
    }

private:
    static QRect expanded_global_geometry(const QWidget* widget) {
        if (widget == nullptr) {
            return {};
        }

        const QRect geometry(widget->mapToGlobal(QPoint(0, 0)), widget->size());
        return geometry.adjusted(-kMenuHoverTolerancePx,
                                 -kMenuHoverTolerancePx,
                                 kMenuHoverTolerancePx,
                                 kMenuHoverTolerancePx);
    }

    bool cursor_inside_visible_menu() const {
        const QPoint cursor_pos = QCursor::pos();
        for (const auto& menu_ptr : visible_menus_) {
            MenuPersistente* menu = menu_ptr.data();
            if (menu != nullptr && menu->isVisible() &&
                expanded_global_geometry(menu).contains(cursor_pos)) {
                return true;
            }
        }
        return false;
    }

    void prune_hidden_menus() {
        for (auto it = visible_menus_.begin(); it != visible_menus_.end();) {
            MenuPersistente* menu = it->data();
            if (menu == nullptr || !menu->isVisible()) {
                it = visible_menus_.erase(it);
            } else {
                ++it;
            }
        }
    }

    void reevaluate_close_state() {
        prune_hidden_menus();
        ensure_monitoring();

        if (visible_menus_.isEmpty()) {
            close_timer_.stop();
            return;
        }

        if (cursor_inside_visible_menu()) {
            close_timer_.stop();
            return;
        }

        if (!close_timer_.isActive()) {
            close_timer_.start();
        }
    }

    void ensure_monitoring() {
        if (visible_menus_.isEmpty()) {
            monitor_timer_.stop();
            close_timer_.stop();
            return;
        }
        if (!monitor_timer_.isActive()) {
            monitor_timer_.start();
        }
    }

    void close_all_visible_menus() {
        prune_hidden_menus();
        if (visible_menus_.isEmpty()) {
            monitor_timer_.stop();
            close_timer_.stop();
            return;
        }

        const auto menus = visible_menus_;
        visible_menus_.clear();
        monitor_timer_.stop();
        close_timer_.stop();

        for (const auto& menu_ptr : menus) {
            if (MenuPersistente* menu = menu_ptr.data(); menu != nullptr) {
                menu->hide();
            }
        }
    }

    void close_visible_menus_if_needed() {
        prune_hidden_menus();
        if (visible_menus_.isEmpty() || cursor_inside_visible_menu()) {
            return;
        }
        close_all_visible_menus();
    }

    QList<QPointer<MenuPersistente>> visible_menus_;
    QTimer monitor_timer_;
    QTimer close_timer_;
};

MenuHoverTracker& hover_tracker() {
    static MenuHoverTracker tracker;
    return tracker;
}

}  // namespace

MenuPersistente::MenuPersistente(const QString& titulo, QWidget* parent)
    : QMenu(titulo, parent) {
    try {
        setAttribute(Qt::WA_DeleteOnClose, false);
        setMouseTracking(true);
        hover_tracker().note_cursor_activity();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar MenuPersistente", true);
    }
}

void MenuPersistente::enterEvent(QEnterEvent* event) {
    try {
        notify_cursor_activity();
        QMenu::enterEvent(event);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro no enterEvent", true);
    }
}

void MenuPersistente::hideEvent(QHideEvent* event) {
    try {
        hover_tracker().unregister_menu(this);
        QMenu::hideEvent(event);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro no hideEvent", true);
    }
}

void MenuPersistente::mousePressEvent(QMouseEvent* event) {
    try {
        notify_cursor_activity();
        QMenu::mousePressEvent(event);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro no mousePressEvent", true);
    }
}

void MenuPersistente::mouseMoveEvent(QMouseEvent* event) {
    try {
        notify_cursor_activity();
        QMenu::mouseMoveEvent(event);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro no mouseMoveEvent", true);
    }
}

void MenuPersistente::leaveEvent(QEvent* event) {
    try {
        notify_cursor_activity();
        QMenu::leaveEvent(event);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro no leaveEvent", true);
    }
}

void MenuPersistente::mouseReleaseEvent(QMouseEvent* event) {
    try {
        notify_cursor_activity();
        if (event != nullptr && event->button() == Qt::LeftButton) {
            QAction* action = actionAt(event->pos());
            if (action != nullptr && action->isEnabled() && !action->isSeparator()) {
                setActiveAction(action);

                if (action->menu() == nullptr) {
                    QPointer<QAction> action_guard(action);
                    event->accept();
                    action->trigger();
                    if (action_guard != nullptr) {
                        setActiveAction(action_guard);
                    }
                    update();
                    notify_cursor_activity();
                    return;
                }
            }
        }
        QMenu::mouseReleaseEvent(event);
        notify_cursor_activity();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro no mouseReleaseEvent", true);
    }
}

void MenuPersistente::showEvent(QShowEvent* event) {
    try {
        hover_tracker().register_menu(this);
        QMenu::showEvent(event);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro no showEvent", true);
    }
}

void MenuPersistente::notify_cursor_activity() { hover_tracker().note_cursor_activity(); }

}
