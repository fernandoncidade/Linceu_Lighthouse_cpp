#pragma once

#include <QDialog>

#include "source/security/secui_00_helpers.h"

class QCheckBox;
class QDialogButtonBox;
class QLabel;
class QSpinBox;

namespace source::security::ui {

class NotificationPopupConfigDialog : public QDialog {
public:
    NotificationPopupConfigDialog(QWidget* parent,
                                  QObject* interface_obj,
                                  const QString& popup_key,
                                  const QString& popup_label_key,
                                  const QString& popup_label_fallback,
                                  const QString& threshold_unit_key,
                                  const QString& threshold_unit_fallback,
                                  bool default_enabled);

    SecurityPopupConfig config() const;

public slots:
    void refresh_texts();

private:
    void refresh_control_state();

private:
    QObject* interface_;
    QObject* loc_;
    QString popup_key_;
    QString popup_label_key_;
    QString popup_label_fallback_;
    QString threshold_unit_key_;
    QString threshold_unit_fallback_;
    bool default_enabled_;
    QLabel* hint_label_;
    QCheckBox* enabled_checkbox_;
    QLabel* threshold_label_;
    QSpinBox* threshold_spin_;
    QLabel* max_lines_label_;
    QSpinBox* max_lines_spin_;
    QLabel* cooldown_label_;
    QSpinBox* cooldown_spin_;
    QDialogButtonBox* button_box_;
};

}  // namespace source::security::ui
