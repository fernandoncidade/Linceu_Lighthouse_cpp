#include "ui_14_OpcoesSobre.h"

#include "source/utils/ApplicationPathUtils.h"
#include "source/utils/LogManager.h"

namespace source::ui::OpcoesSobre {

QString SITE_LICENSES = QStringLiteral(
    "https://www.gnu.org/licenses/lgpl-3.0.html.en\n"
    "https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.en\n"
    "https://opensource.org/licenses/BSD-2-Clause\n"
    "https://opensource.org/licenses/BSD-3-Clause\n"
    "https://www.apache.org/licenses/LICENSE-2.0\n"
    "https://opensource.org/licenses/MIT\n"
    "https://www.sqlite.org/copyright.html\n"
    "https://opensource.org/license/isc-license-txt\n");

QString GNU_LGPL_V3_LICENSE_PATH =
    QStringLiteral("source/assets/LICENSES/GNU LESSER GENERAL PUBLIC LICENSE Version 3.txt");
QString GNU_LGPL_V3_LICENSE_TEXT;

QString LICENSE_TEXT_PT_BR;
QString LICENSE_TEXT_EN_US;
QString LICENSE_TEXT_ES_ES;
QString LICENSE_TEXT_FR_FR;
QString LICENSE_TEXT_IT_IT;
QString LICENSE_TEXT_DE_DE;

QString NOTICE_TEXT_PT_BR;
QString NOTICE_TEXT_EN_US;
QString NOTICE_TEXT_ES_ES;
QString NOTICE_TEXT_FR_FR;
QString NOTICE_TEXT_IT_IT;
QString NOTICE_TEXT_DE_DE;

QString ABOUT_TEXT_PT_BR;
QString ABOUT_TEXT_EN_US;
QString ABOUT_TEXT_ES_ES;
QString ABOUT_TEXT_FR_FR;
QString ABOUT_TEXT_IT_IT;
QString ABOUT_TEXT_DE_DE;

QString PRIVACY_POLICY_PT_BR;
QString PRIVACY_POLICY_EN_US;
QString PRIVACY_POLICY_ES_ES;
QString PRIVACY_POLICY_FR_FR;
QString PRIVACY_POLICY_IT_IT;
QString PRIVACY_POLICY_DE_DE;

QString HISTORY_APP_PT_BR;
QString HISTORY_APP_EN_US;
QString HISTORY_APP_ES_ES;
QString HISTORY_APP_FR_FR;
QString HISTORY_APP_IT_IT;
QString HISTORY_APP_DE_DE;

QString RELEASE_NOTES_PT_BR;
QString RELEASE_NOTES_EN_US;
QString RELEASE_NOTES_ES_ES;
QString RELEASE_NOTES_FR_FR;
QString RELEASE_NOTES_IT_IT;
QString RELEASE_NOTES_DE_DE;

void carregar_conteudos() {
    try {
        GNU_LGPL_V3_LICENSE_TEXT =
            source::utils::load_text_file("GNU LESSER GENERAL PUBLIC LICENSE Version 3.txt", "LICENSES")
                .value_or(QString());

        LICENSE_TEXT_PT_BR = source::utils::load_text_file("EULA_pt_BR - Linceu Lighthouse.txt", "EULA").value_or(QString());
        LICENSE_TEXT_EN_US = source::utils::load_text_file("EULA_en_US - Linceu Lighthouse.txt", "EULA").value_or(QString());
        LICENSE_TEXT_ES_ES = source::utils::load_text_file("EULA_es_ES - Linceu Lighthouse.txt", "EULA").value_or(QString());
        LICENSE_TEXT_FR_FR = source::utils::load_text_file("EULA_fr_FR - Linceu Lighthouse.txt", "EULA").value_or(QString());
        LICENSE_TEXT_IT_IT = source::utils::load_text_file("EULA_it_IT - Linceu Lighthouse.txt", "EULA").value_or(QString());
        LICENSE_TEXT_DE_DE = source::utils::load_text_file("EULA_de_DE - Linceu Lighthouse.txt", "EULA").value_or(QString());

        NOTICE_TEXT_PT_BR = source::utils::load_text_file("NOTICE_pt_BR.txt", "NOTICES").value_or(QString());
        NOTICE_TEXT_EN_US = source::utils::load_text_file("NOTICE_en_US.txt", "NOTICES").value_or(QString());
        NOTICE_TEXT_ES_ES = source::utils::load_text_file("NOTICE_es_ES.txt", "NOTICES").value_or(QString());
        NOTICE_TEXT_FR_FR = source::utils::load_text_file("NOTICE_fr_FR.txt", "NOTICES").value_or(QString());
        NOTICE_TEXT_IT_IT = source::utils::load_text_file("NOTICE_it_IT.txt", "NOTICES").value_or(QString());
        NOTICE_TEXT_DE_DE = source::utils::load_text_file("NOTICE_de_DE.txt", "NOTICES").value_or(QString());

        ABOUT_TEXT_PT_BR = source::utils::load_text_file("ABOUT_pt_BR.txt", "ABOUT").value_or(QString());
        ABOUT_TEXT_EN_US = source::utils::load_text_file("ABOUT_en_US.txt", "ABOUT").value_or(QString());
        ABOUT_TEXT_ES_ES = source::utils::load_text_file("ABOUT_es_ES.txt", "ABOUT").value_or(QString());
        ABOUT_TEXT_FR_FR = source::utils::load_text_file("ABOUT_fr_FR.txt", "ABOUT").value_or(QString());
        ABOUT_TEXT_IT_IT = source::utils::load_text_file("ABOUT_it_IT.txt", "ABOUT").value_or(QString());
        ABOUT_TEXT_DE_DE = source::utils::load_text_file("ABOUT_de_DE.txt", "ABOUT").value_or(QString());

        PRIVACY_POLICY_PT_BR = source::utils::load_text_file("Privacy_Policy_pt_BR.txt", "PRIVACY_POLICY").value_or(QString());
        PRIVACY_POLICY_EN_US = source::utils::load_text_file("Privacy_Policy_en_US.txt", "PRIVACY_POLICY").value_or(QString());
        PRIVACY_POLICY_ES_ES = source::utils::load_text_file("Privacy_Policy_es_ES.txt", "PRIVACY_POLICY").value_or(QString());
        PRIVACY_POLICY_FR_FR = source::utils::load_text_file("Privacy_Policy_fr_FR.txt", "PRIVACY_POLICY").value_or(QString());
        PRIVACY_POLICY_IT_IT = source::utils::load_text_file("Privacy_Policy_it_IT.txt", "PRIVACY_POLICY").value_or(QString());
        PRIVACY_POLICY_DE_DE = source::utils::load_text_file("Privacy_Policy_de_DE.txt", "PRIVACY_POLICY").value_or(QString());

        HISTORY_APP_PT_BR = source::utils::load_text_file("History_APP_pt_BR.txt", "ABOUT").value_or(QString());
        HISTORY_APP_EN_US = source::utils::load_text_file("History_APP_en_US.txt", "ABOUT").value_or(QString());
        HISTORY_APP_ES_ES = source::utils::load_text_file("History_APP_es_ES.txt", "ABOUT").value_or(QString());
        HISTORY_APP_FR_FR = source::utils::load_text_file("History_APP_fr_FR.txt", "ABOUT").value_or(QString());
        HISTORY_APP_IT_IT = source::utils::load_text_file("History_APP_it_IT.txt", "ABOUT").value_or(QString());
        HISTORY_APP_DE_DE = source::utils::load_text_file("History_APP_de_DE.txt", "ABOUT").value_or(QString());

        RELEASE_NOTES_PT_BR = source::utils::load_text_file("RELEASE NOTES_pt_BR.txt", "RELEASE").value_or(QString());
        RELEASE_NOTES_EN_US = source::utils::load_text_file("RELEASE NOTES_en_US.txt", "RELEASE").value_or(QString());
        RELEASE_NOTES_ES_ES = source::utils::load_text_file("RELEASE NOTES_es_ES.txt", "RELEASE").value_or(QString());
        RELEASE_NOTES_FR_FR = source::utils::load_text_file("RELEASE NOTES_fr_FR.txt", "RELEASE").value_or(QString());
        RELEASE_NOTES_IT_IT = source::utils::load_text_file("RELEASE NOTES_it_IT.txt", "RELEASE").value_or(QString());
        RELEASE_NOTES_DE_DE = source::utils::load_text_file("RELEASE NOTES_de_DE.txt", "RELEASE").value_or(QString());
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao carregar arquivos de licença", true);
    }
}

}  // namespace source::ui::OpcoesSobre
