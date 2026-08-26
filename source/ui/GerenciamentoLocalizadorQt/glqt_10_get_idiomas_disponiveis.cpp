#include "glqt_10_get_idiomas_disponiveis.h"

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoLocalizadorQt {

QHash<QString, QString> get_idiomas_disponiveis(source::ui::LocalizadorQt* self) {
    try {
        if (self->idioma_atual_ == "pt_BR") {
            return {{"pt_BR", "Português Brasileiro | Brazilian Portuguese"}, {"en_US", "Inglês (EUA) | English (US)"},
                    {"es_ES", "Espanhol | Español"}, {"fr_FR", "Francês | Français"},
                    {"it_IT", "Italiano | Italiano"}, {"de_DE", "Alemão | Deutsch"}};
        }
        if (self->idioma_atual_ == "en_US") {
            return {{"pt_BR", "Brazilian Portuguese | Português Brasileiro"}, {"en_US", "English (US) | Inglês (US)"},
                    {"es_ES", "Spanish | Español"}, {"fr_FR", "French | Français"},
                    {"it_IT", "Italian | Italiano"}, {"de_DE", "German | Deutsch"}};
        }
        if (self->idioma_atual_ == "es_ES") {
            return {{"pt_BR", "Portugués Brasileño | Português Brasileiro"}, {"en_US", "Inglés (EEUU) | English (US)"},
                    {"es_ES", "Español | Spanish"}, {"fr_FR", "Francés | Français"},
                    {"it_IT", "Italiano | Italiano"}, {"de_DE", "Alemán | Deutsch"}};
        }
        if (self->idioma_atual_ == "fr_FR") {
            return {{"pt_BR", "Portugais Brésilien | Português Brasileiro"}, {"en_US", "Anglais (États-Unis) | English (US)"},
                    {"es_ES", "Espagnol | Español"}, {"fr_FR", "Français | French"},
                    {"it_IT", "Italien | Italiano"}, {"de_DE", "Allemand | Deutsch"}};
        }
        if (self->idioma_atual_ == "it_IT") {
            return {{"pt_BR", "Portoghese Brasiliano | Português Brasileiro"}, {"en_US", "Inglese (Stati Uniti) | English (US)"},
                    {"es_ES", "Spagnolo | Español"}, {"fr_FR", "Francese | Français"},
                    {"it_IT", "Italiano | Italian"}, {"de_DE", "Tedesco | Deutsch"}};
        }
        if (self->idioma_atual_ == "de_DE") {
            return {{"pt_BR", "Brasilianisches Portugiesisch | Português Brasileiro"}, {"en_US", "Englisch (USA) | English (US)"},
                    {"es_ES", "Spanisch | Español"}, {"fr_FR", "Französisch | Français"},
                    {"it_IT", "Italienisch | Italiano"}, {"de_DE", "Deutsch | German"}};
        }
        return self->idiomas_suportados_;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter idiomas disponíveis", true);
        return self->idiomas_suportados_;
    }
}

}
