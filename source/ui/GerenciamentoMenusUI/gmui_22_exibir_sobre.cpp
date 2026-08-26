#include "gmui_22_exibir_sobre.h"

#include <QMessageBox>
#include <QMetaObject>
#include <QVariant>

#include "gmui_03_SobreDialog.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/ui/ui_14_OpcoesSobre.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString text = fallback;
    if (loc != nullptr) {
        if (!QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key)) &&
            !fallback.isEmpty()) {
            text = fallback;
        }
    }
    return text;
}

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

namespace source::ui::GerenciamentoMenusUI {

void exibir_sobre(source::ui::GerenciadorMenusUI* self) {
    try {
        if (self == nullptr || self->interface_obj() == nullptr || self->loc_obj() == nullptr) {
            return;
        }

        QObject* iface = self->interface_obj();
        QObject* loc = self->loc_obj();

        source::ui::OpcoesSobre::carregar_conteudos();

        const QString idioma = loc->property("idioma_atual").toString();

        const QString texto_sobre = (idioma == QStringLiteral("pt_BR"))   ? source::ui::OpcoesSobre::ABOUT_TEXT_PT_BR
                                  : (idioma == QStringLiteral("es_ES")) ? source::ui::OpcoesSobre::ABOUT_TEXT_ES_ES
                                  : (idioma == QStringLiteral("fr_FR")) ? source::ui::OpcoesSobre::ABOUT_TEXT_FR_FR
                                  : (idioma == QStringLiteral("it_IT")) ? source::ui::OpcoesSobre::ABOUT_TEXT_IT_IT
                                  : (idioma == QStringLiteral("de_DE")) ? source::ui::OpcoesSobre::ABOUT_TEXT_DE_DE
                                                                        : source::ui::OpcoesSobre::ABOUT_TEXT_EN_US;

        const QString texto_licenca = (idioma == QStringLiteral("pt_BR"))   ? source::ui::OpcoesSobre::LICENSE_TEXT_PT_BR
                                    : (idioma == QStringLiteral("es_ES")) ? source::ui::OpcoesSobre::LICENSE_TEXT_ES_ES
                                    : (idioma == QStringLiteral("fr_FR")) ? source::ui::OpcoesSobre::LICENSE_TEXT_FR_FR
                                    : (idioma == QStringLiteral("it_IT")) ? source::ui::OpcoesSobre::LICENSE_TEXT_IT_IT
                                    : (idioma == QStringLiteral("de_DE")) ? source::ui::OpcoesSobre::LICENSE_TEXT_DE_DE
                                                                          : source::ui::OpcoesSobre::LICENSE_TEXT_EN_US;

        const QString texto_aviso = (idioma == QStringLiteral("pt_BR"))   ? source::ui::OpcoesSobre::NOTICE_TEXT_PT_BR
                                  : (idioma == QStringLiteral("es_ES")) ? source::ui::OpcoesSobre::NOTICE_TEXT_ES_ES
                                  : (idioma == QStringLiteral("fr_FR")) ? source::ui::OpcoesSobre::NOTICE_TEXT_FR_FR
                                  : (idioma == QStringLiteral("it_IT")) ? source::ui::OpcoesSobre::NOTICE_TEXT_IT_IT
                                  : (idioma == QStringLiteral("de_DE")) ? source::ui::OpcoesSobre::NOTICE_TEXT_DE_DE
                                                                        : source::ui::OpcoesSobre::NOTICE_TEXT_EN_US;

        const QString texto_privacidade = (idioma == QStringLiteral("pt_BR"))   ? source::ui::OpcoesSobre::PRIVACY_POLICY_PT_BR
                                       : (idioma == QStringLiteral("es_ES")) ? source::ui::OpcoesSobre::PRIVACY_POLICY_ES_ES
                                       : (idioma == QStringLiteral("fr_FR")) ? source::ui::OpcoesSobre::PRIVACY_POLICY_FR_FR
                                       : (idioma == QStringLiteral("it_IT")) ? source::ui::OpcoesSobre::PRIVACY_POLICY_IT_IT
                                       : (idioma == QStringLiteral("de_DE")) ? source::ui::OpcoesSobre::PRIVACY_POLICY_DE_DE
                                                                             : source::ui::OpcoesSobre::PRIVACY_POLICY_EN_US;

        const QString texto_history = (idioma == QStringLiteral("pt_BR"))   ? source::ui::OpcoesSobre::HISTORY_APP_PT_BR
                                    : (idioma == QStringLiteral("es_ES")) ? source::ui::OpcoesSobre::HISTORY_APP_ES_ES
                                    : (idioma == QStringLiteral("fr_FR")) ? source::ui::OpcoesSobre::HISTORY_APP_FR_FR
                                    : (idioma == QStringLiteral("it_IT")) ? source::ui::OpcoesSobre::HISTORY_APP_IT_IT
                                    : (idioma == QStringLiteral("de_DE")) ? source::ui::OpcoesSobre::HISTORY_APP_DE_DE
                                                                          : source::ui::OpcoesSobre::HISTORY_APP_EN_US;

        const QString texto_release_notes = (idioma == QStringLiteral("pt_BR"))   ? source::ui::OpcoesSobre::RELEASE_NOTES_PT_BR
                                          : (idioma == QStringLiteral("es_ES")) ? source::ui::OpcoesSobre::RELEASE_NOTES_ES_ES
                                          : (idioma == QStringLiteral("fr_FR")) ? source::ui::OpcoesSobre::RELEASE_NOTES_FR_FR
                                          : (idioma == QStringLiteral("it_IT")) ? source::ui::OpcoesSobre::RELEASE_NOTES_IT_IT
                                          : (idioma == QStringLiteral("de_DE")) ? source::ui::OpcoesSobre::RELEASE_NOTES_DE_DE
                                                                                : source::ui::OpcoesSobre::RELEASE_NOTES_EN_US;

        const QString cabecalho_fixo = QStringLiteral("<h3>LINCEU LIGHTHOUSE</h3>")
            + QStringLiteral("<p><b>") + get_text(loc, "version", "Version") + QStringLiteral(":</b> 2026.4.27.0</p>")
            + QStringLiteral("<p><b>") + get_text(loc, "authors", "Authors") + QStringLiteral(":</b> Fernando Nillsson Cidade</p>")
            + QStringLiteral("<p><b>") + get_text(loc, "description", "Description") + QStringLiteral(":</b> ")
            + get_text(loc, "description_text", "Application information") + QStringLiteral("</p>");

        auto* dialog = new SobreDialog(
            nullptr,
            iface,
            get_text(loc, "about", "About") + QStringLiteral(" - LINCEU LIGHTHOUSE"),
            cabecalho_fixo,
            texto_history,
            texto_sobre,
            texto_licenca,
            source::ui::OpcoesSobre::SITE_LICENSES,
            get_text(loc, "show_history", "Show"),
            get_text(loc, "hide_history", "Hide"),
            get_text(loc, "show_details", "Show"),
            get_text(loc, "hide_details", "Hide"),
            get_text(loc, "show_licenses", "Show"),
            get_text(loc, "hide_licenses", "Hide"),
            get_text(loc, "ok", "OK"),
            get_text(loc, "site_oficial", "Official site"),
            texto_aviso,
            get_text(loc, "show_notices", "Show"),
            get_text(loc, "hide_notices", "Hide"),
            texto_privacidade,
            get_text(loc, "show_privacy_policy", "Show"),
            get_text(loc, "hide_privacy_policy", "Hide"),
            get_text(loc, "information_not_available", "Information not available"),
            texto_release_notes,
            get_text(loc, "show_release_notes", "Show"),
            get_text(loc, "hide_release_notes", "Hide"));

        const int largura_dialog = static_cast<int>(900 * 1.0);
        const int altura_dialog = static_cast<int>(500 * 1.8);
        dialog->resize(largura_dialog, altura_dialog);

        registrar_dialogo_na_interface(iface, "_sobre_dialog", dialog);
        dialog->show();
        dialog->raise();
        dialog->activateWindow();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao exibir o diálogo 'Sobre'", true);
        if (self != nullptr && self->interface_obj() != nullptr && self->loc_obj() != nullptr) {
            QMessageBox::critical(
                qobject_cast<QWidget*>(self->interface_obj()),
                get_text(self->loc_obj(), "error", "Error"),
                get_text(self->loc_obj(), "error", "Error"));
        }
    }
}

}  // namespace source::ui::GerenciamentoMenusUI
