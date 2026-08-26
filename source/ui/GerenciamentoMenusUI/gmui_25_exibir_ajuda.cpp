#include "gmui_25_exibir_ajuda.h"

#include <QHBoxLayout>
#include <QMetaObject>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>

#include "source/ui/Manual/common_Manual.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoMenusUI {

namespace {
void registrar_dialogo_na_interface(QObject* iface, const char* property_name, QWidget* dialog) {
    if (iface == nullptr || property_name == nullptr || dialog == nullptr) {
        return;
    }

    dialog->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(dialog, &QObject::destroyed, iface, [iface, property_name](QObject* dialog_destruido) {
        QObject* dialog_atual = iface->property(property_name).value<QObject*>();
        if (dialog_atual == dialog_destruido) {
            iface->setProperty(property_name, QVariant::fromValue(static_cast<QObject*>(nullptr)));
        }
    });
    iface->setProperty(property_name, QVariant::fromValue(static_cast<QObject*>(dialog)));
}
}  // namespace

AjudaDialog::AjudaDialog(QWidget* parent, QObject* interface_ref)
    : QDialog(parent),
      parent_ref_(interface_ref != nullptr ? interface_ref : parent),
      loc_(parent_ref_ != nullptr ? parent_ref_->property("loc").value<QObject*>() : nullptr),
      browser_(nullptr),
      btn_ok_(nullptr) {
    try {
        setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                       Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint |
                       Qt::WindowCloseButtonHint);
        setModal(false);

        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(6, 6, 6, 6);

        browser_ = new QTextBrowser(this);
        browser_->setOpenExternalLinks(true);
        layout->addWidget(browser_);

        auto* footer = new QWidget(this);
        auto* footer_layout = new QHBoxLayout(footer);
        footer_layout->addStretch();
        btn_ok_ = new QPushButton(tr("OK"), this);
        QObject::connect(btn_ok_, &QPushButton::clicked, this, &QDialog::close);
        footer_layout->addWidget(btn_ok_);
        layout->addWidget(footer);

        resize(640, 360);

        if (loc_ != nullptr) {
            QObject::connect(loc_, SIGNAL(idioma_alterado(QString)), this, SLOT(on_idioma_alterado(QString)));
        }

        build_content();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar AjudaDialog", true);
    }
}

QString AjudaDialog::t(const QString& key, const QString& fallback) const {
    try {
        if (loc_ != nullptr) {
            QString out = fallback;
            if (QMetaObject::invokeMethod(loc_, "get_text", Q_RETURN_ARG(QString, out), Q_ARG(QString, key))) {
                if (!out.isEmpty()) {
                    return out;
                }
            }
        }
    } catch (...) {
    }
    return fallback;
}

void AjudaDialog::build_content() {
    try {
        if (browser_ == nullptr) {
            return;
        }

        const QString lang = (loc_ != nullptr) ? loc_->property("idioma_atual").toString() : QStringLiteral("pt_BR");
        const auto atalhos = source::ui::manual::shortcut_entries(lang);

        QStringList html_lines;
        html_lines << QStringLiteral("<h3>%1</h3>").arg(t("keyboard_shortcuts", "Atalhos de teclado"));
        html_lines << QStringLiteral("<table cellpadding=6>");
        for (const auto& atalho : atalhos) {
            html_lines << QStringLiteral("<tr><td><b>%1</b></td><td>%2</td></tr>")
                              .arg(atalho.sequence, atalho.description);
        }
        html_lines << QStringLiteral("</table>");

        setWindowTitle(t("help", "Ajuda"));
        if (btn_ok_ != nullptr) {
            btn_ok_->setText(tr("OK"));
        }

        browser_->setHtml(html_lines.join('\n'));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao construir conteúdo AjudaDialog", true);
    }
}

void AjudaDialog::on_idioma_alterado(const QString&) {
    try {
        if (parent_ref_ != nullptr) {
            loc_ = parent_ref_->property("loc").value<QObject*>();
        }
        build_content();
    } catch (...) {
    }
}

void exibir_ajuda(source::ui::GerenciadorMenusUI* self) {
    try {
        if (self == nullptr || self->interface_obj() == nullptr) {
            return;
        }

        QObject* iface = self->interface_obj();
        auto* dialog = new AjudaDialog(nullptr, iface);

        registrar_dialogo_na_interface(iface, "_ajuda_dialog", dialog);
        dialog->show();
        dialog->raise();
        dialog->activateWindow();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao exibir Ajuda", true);
    }
}

}  // namespace source::ui::GerenciamentoMenusUI
