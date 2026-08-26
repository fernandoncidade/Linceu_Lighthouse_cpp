#include "gmui_24_exibir_manual.h"

#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMetaObject>
#include <QPushButton>
#include <QSizePolicy>
#include <QSplitter>
#include <QTextBrowser>
#include <QVBoxLayout>

#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/ui/ui_15_Manual.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoMenusUI {

namespace {
const QString kManualTopAnchor = QStringLiteral("manual_top");

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

ManualDialog::ManualDialog(QWidget* parent, QObject* interface_ref, const QString& titulo, const QString& lang)
    : QDialog(parent),
      toc_list_(nullptr),
      browser_(nullptr),
      btn_ok_(nullptr),
      current_lang_(lang),
      parent_ref_(interface_ref != nullptr ? interface_ref : parent),
      loc_(parent_ref_ != nullptr ? parent_ref_->property("loc").value<QObject*>() : nullptr) {
    try {
        setWindowTitle(titulo);
        setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                       Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint |
                       Qt::WindowMaximizeButtonHint);
        setModal(false);

        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(6, 6, 6, 6);

        auto* splitter = new QSplitter(Qt::Horizontal, this);

        toc_list_ = new QListWidget(this);
        toc_list_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        splitter->addWidget(toc_list_);

        browser_ = new QTextBrowser(this);
        browser_->setOpenExternalLinks(true);
        browser_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        splitter->addWidget(browser_);

        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 3);
        splitter->setCollapsible(0, false);
        splitter->setCollapsible(1, false);
        splitter->setSizes({280, 820});

        layout->addWidget(splitter, 1);

        auto* footer_layout = new QHBoxLayout();
        footer_layout->addStretch();
        btn_ok_ = new QPushButton(tr("OK"), this);
        QObject::connect(btn_ok_, &QPushButton::clicked, this, &QDialog::close);
        footer_layout->addWidget(btn_ok_);
        layout->addLayout(footer_layout);

        setSizeGripEnabled(true);
        setMinimumSize(700, 480);
        resize(1000, 720);

        if (current_lang_.isEmpty()) {
            current_lang_ = (loc_ != nullptr) ? loc_->property("idioma_atual").toString() : QStringLiteral("pt_BR");
            if (current_lang_.isEmpty()) {
                current_lang_ = QStringLiteral("pt_BR");
            }
        }

        build_manual_content(current_lang_);

        if (loc_ != nullptr) {
            QObject::connect(loc_, SIGNAL(idioma_alterado(QString)), this, SLOT(on_idioma_alterado(QString)));
        }

        QObject::connect(toc_list_, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
            if (item == nullptr || browser_ == nullptr) {
                return;
            }

            const QString section_id = item->data(Qt::UserRole).toString();
            if (!section_id.isEmpty()) {
                browser_->scrollToAnchor(section_id);
            }
        });
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar ManualDialog", true);
    }
}

void ManualDialog::on_idioma_alterado(const QString& novo_idioma) {
    try {
        current_lang_ = novo_idioma;
        setWindowTitle(source::ui::manual::get_manual_title(novo_idioma));
        if (btn_ok_ != nullptr) {
            btn_ok_->setText(tr("OK"));
        }
        build_manual_content(novo_idioma);
    } catch (...) {
    }
}

void ManualDialog::build_manual_content(const QString& lang) {
    try {
        if (toc_list_ == nullptr || browser_ == nullptr) {
            return;
        }

        const auto [blocks, order] = source::ui::manual::get_manual_blocks(lang);
        Q_UNUSED(order)

        toc_list_->clear();
        QStringList html_parts;
        const QString link_color = browser_->palette().color(QPalette::Link).name();

        for (const auto& block : blocks) {
            if (block.kind == QStringLiteral("toc_item") && !block.section_id.isEmpty()) {
                auto* item = new QListWidgetItem(block.text, toc_list_);
                item->setData(Qt::UserRole, block.section_id);
            }
        }

        bool manual_title_rendered = false;
        for (const auto& block : blocks) {
            if (block.kind == QStringLiteral("bullet")) {
                html_parts << QStringLiteral("<ul><li>%1</li></ul>").arg(block.text);
            } else if (block.kind == QStringLiteral("toc_title")) {
                html_parts << QStringLiteral("<h3>%1</h3>").arg(block.text);
            } else if (block.kind == QStringLiteral("toc_item")) {
                html_parts << QStringLiteral("<p><a href=\"#%1\">%2</a></p>").arg(block.section_id, block.text);
            } else if (block.kind == QStringLiteral("section_title")) {
                html_parts << QStringLiteral(
                                  "<h2 id=\"%1\"><a href=\"#%2\" style=\"color: %3; text-decoration: none;\">%4</a></h2>")
                                  .arg(block.section_id, kManualTopAnchor, link_color, block.text);
            } else if (block.kind == QStringLiteral("detail_title")) {
                html_parts << QStringLiteral("<h4>%1</h4>").arg(block.text);
            } else if (block.kind == QStringLiteral("paragraph") || block.kind == QStringLiteral("line")) {
                if (!manual_title_rendered && block.kind == QStringLiteral("line")) {
                    html_parts << QStringLiteral("<p id=\"%1\"><a name=\"%1\"></a>%2</p>")
                                      .arg(kManualTopAnchor, block.text);
                    manual_title_rendered = true;
                } else {
                    html_parts << QStringLiteral("<p>%1</p>").arg(block.text);
                }
            } else if (block.kind == QStringLiteral("divider")) {
                html_parts << QStringLiteral("<hr />");
            } else if (block.kind == QStringLiteral("blank")) {
                html_parts << QString();
            }
        }

        browser_->setHtml(html_parts.join('\n'));
    } catch (...) {
    }
}

void exibir_manual(source::ui::GerenciadorMenusUI* self) {
    try {
        if (self == nullptr || self->interface_obj() == nullptr || self->loc_obj() == nullptr) {
            return;
        }

        QObject* iface = self->interface_obj();
        QObject* loc = self->loc_obj();

        QString lang = loc->property("idioma_atual").toString();
        if (lang.isEmpty()) {
            lang = QStringLiteral("pt_BR");
        }

        const QString titulo = source::ui::manual::get_manual_title(lang);
        auto* dialog = new ManualDialog(nullptr, iface, titulo, lang);

        registrar_dialogo_na_interface(iface, "_manual_dialog", dialog);
        dialog->show();
        dialog->raise();
        dialog->activateWindow();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao exibir manual", true);
    }
}

}  // namespace source::ui::GerenciamentoMenusUI
