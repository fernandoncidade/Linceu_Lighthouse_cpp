#pragma once

#include <QMenu>

class QEnterEvent;
class QHideEvent;
class QMouseEvent;
class QShowEvent;

namespace source::ui::GerenciamentoMenusUI {

class MenuPersistente : public QMenu {
public:
    explicit MenuPersistente(const QString& titulo, QWidget* parent = nullptr);

protected:
    void enterEvent(QEnterEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void notify_cursor_activity();
};

}
