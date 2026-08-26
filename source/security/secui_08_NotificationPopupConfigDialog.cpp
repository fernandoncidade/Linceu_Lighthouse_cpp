#include "secui_08_NotificationPopupConfigDialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include "source/ui/ui_12_LocalizadorQt.h"

namespace source::security::ui {

NotificationPopupConfigDialog::NotificationPopupConfigDialog(QWidget* parent,
                                                             QObject* interface_obj,
                                                             const QString& popup_key,
                                                             const QString& popup_label_key,
                                                             const QString& popup_label_fallback,
                                                             const QString& threshold_unit_key,
                                                             const QString& threshold_unit_fallback,
                                                             bool default_enabled)
    : QDialog(parent),
      interface_(interface_obj),
      loc_(get_localizador(interface_obj)),
      popup_key_(popup_key),
      popup_label_key_(popup_label_key),
      popup_label_fallback_(popup_label_fallback),
      threshold_unit_key_(threshold_unit_key),
      threshold_unit_fallback_(threshold_unit_fallback),
      default_enabled_(default_enabled),
      hint_label_(nullptr),
      enabled_checkbox_(nullptr),
      threshold_label_(nullptr),
      threshold_spin_(nullptr),
      max_lines_label_(nullptr),
      max_lines_spin_(nullptr),
      cooldown_label_(nullptr),
      cooldown_spin_(nullptr),
      button_box_(nullptr) {
    setModal(true);
    setMinimumWidth(460);

    auto* root_layout = new QVBoxLayout(this);
    root_layout->setContentsMargins(12, 12, 12, 12);
    root_layout->setSpacing(10);

    hint_label_ = new QLabel(this);
    hint_label_->setWordWrap(true);
    root_layout->addWidget(hint_label_);

    enabled_checkbox_ = new QCheckBox(this);
    root_layout->addWidget(enabled_checkbox_);

    auto* form_layout = new QFormLayout();
    form_layout->setContentsMargins(0, 0, 0, 0);
    form_layout->setSpacing(8);

    threshold_label_ = new QLabel(this);
    threshold_spin_ = new QSpinBox(this);
    threshold_spin_->setRange(1, 1000000);
    threshold_spin_->setAccelerated(true);
    form_layout->addRow(threshold_label_, threshold_spin_);

    max_lines_label_ = new QLabel(this);
    max_lines_spin_ = new QSpinBox(this);
    max_lines_spin_->setRange(1, kSecurityPopupMaxLinesLimit);
    max_lines_spin_->setAccelerated(true);
    form_layout->addRow(max_lines_label_, max_lines_spin_);

    cooldown_label_ = new QLabel(this);
    cooldown_spin_ = new QSpinBox(this);
    cooldown_spin_->setRange(0, 86400);
    cooldown_spin_->setAccelerated(true);
    form_layout->addRow(cooldown_label_, cooldown_spin_);

    root_layout->addLayout(form_layout);

    button_box_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    root_layout->addWidget(button_box_);

    QObject::connect(button_box_, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(button_box_, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(enabled_checkbox_, &QCheckBox::toggled, this,
                     [this](bool) { refresh_control_state(); });

    if (auto* localizador = get_localizador(interface_)) {
        QObject::connect(localizador, &source::ui::LocalizadorQt::idioma_alterado, this,
                         [this](const QString&) { refresh_texts(); });
        QObject::connect(localizador, &source::ui::LocalizadorQt::traducoes_carregadas, this,
                         [this](const QString&) { refresh_texts(); });
    }

    const SecurityPopupConfig stored_config =
        get_security_popup_config(interface_, popup_key_, default_enabled_);
    enabled_checkbox_->setChecked(stored_config.enabled);
    threshold_spin_->setValue(stored_config.threshold_count);
    max_lines_spin_->setValue(stored_config.max_lines);
    cooldown_spin_->setValue(stored_config.cooldown_seconds);

    refresh_texts();
    refresh_control_state();
}

SecurityPopupConfig NotificationPopupConfigDialog::config() const {
    SecurityPopupConfig result;
    result.enabled = enabled_checkbox_ != nullptr && enabled_checkbox_->isChecked();
    result.threshold_count = threshold_spin_ != nullptr ? threshold_spin_->value() : 1;
    result.max_lines = max_lines_spin_ != nullptr ? max_lines_spin_->value() : 4;
    result.cooldown_seconds = cooldown_spin_ != nullptr ? cooldown_spin_->value() : 0;
    return result;
}

void NotificationPopupConfigDialog::refresh_texts() {
    const QString popup_label = get_text(loc_, popup_label_key_, popup_label_fallback_);
    setWindowTitle(
        get_text(loc_, QStringLiteral("security_popup_config_dialog_title"), QStringLiteral("Configurar %1"))
            .arg(popup_label));

    hint_label_->setText(
        get_text(loc_,
                 QStringLiteral("security_popup_config_hint"),
                 QStringLiteral("Defina o limite de eventos acumulados, o número máximo de linhas no resumo e o "
                                "intervalo mínimo entre pop-ups. Use 0 segundos para desativar o cooldown.")));
    enabled_checkbox_->setText(
        get_text(loc_,
                 QStringLiteral("security_popup_config_enabled"),
                 QStringLiteral("Ativar este pop-up")));
    threshold_label_->setText(
        get_text(loc_,
                 QStringLiteral("security_popup_config_threshold"),
                 QStringLiteral("Disparar ao atingir")));
    threshold_spin_->setSuffix(
        QStringLiteral(" %1").arg(get_text(loc_, threshold_unit_key_, threshold_unit_fallback_)));
    max_lines_label_->setText(
        get_text(loc_,
                 QStringLiteral("security_popup_config_max_lines"),
                 QStringLiteral("Máximo de linhas no resumo")));
    cooldown_label_->setText(
        get_text(loc_,
                 QStringLiteral("security_popup_config_cooldown"),
                 QStringLiteral("Cooldown entre pop-ups")));
    cooldown_spin_->setSuffix(
        QStringLiteral(" %1")
            .arg(get_text(loc_,
                          QStringLiteral("security_popup_config_cooldown_suffix"),
                          QStringLiteral("segundos"))));

    if (auto* ok_button = qobject_cast<QPushButton*>(button_box_->button(QDialogButtonBox::Ok))) {
        ok_button->setText(get_text(loc_, QStringLiteral("ok"), QStringLiteral("OK")));
    }
    if (auto* cancel_button =
            qobject_cast<QPushButton*>(button_box_->button(QDialogButtonBox::Cancel))) {
        cancel_button->setText(get_text(loc_, QStringLiteral("cancel"), QStringLiteral("Cancelar")));
    }
}

void NotificationPopupConfigDialog::refresh_control_state() {
    const bool enabled = enabled_checkbox_ != nullptr && enabled_checkbox_->isChecked();
    if (threshold_label_ != nullptr) {
        threshold_label_->setEnabled(enabled);
    }
    if (threshold_spin_ != nullptr) {
        threshold_spin_->setEnabled(enabled);
    }
    if (max_lines_label_ != nullptr) {
        max_lines_label_->setEnabled(enabled);
    }
    if (max_lines_spin_ != nullptr) {
        max_lines_spin_->setEnabled(enabled);
    }
    if (cooldown_label_ != nullptr) {
        cooldown_label_->setEnabled(enabled);
    }
    if (cooldown_spin_ != nullptr) {
        cooldown_spin_->setEnabled(enabled);
    }
}

}  // namespace source::security::ui
