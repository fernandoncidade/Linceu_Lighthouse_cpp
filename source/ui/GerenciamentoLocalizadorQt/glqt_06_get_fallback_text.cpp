#include "glqt_06_get_fallback_text.h"

#include <QHash>

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoLocalizadorQt {

QString get_fallback_text(source::ui::LocalizadorQt*, const QString& key) {
    try {
        static const QHash<QString, QString> fallbacks = {
            {"ms", "ms"}, {"s", "s"}, {"min", "min"}, {"h", "h"}, {"d", "d"},
            {"MS", "ms"}, {"S", "s"}, {"MIN", "min"}, {"H", "h"}, {"D", "d"},
            {"select_all_columns", "Select All Columns"},
            {"deselect_all_columns", "Deselect All Columns"},
            {"readonly", "Read Only"}, {"hidden", "Hidden"}, {"system", "System"},
            {"archive", "Archive"}, {"encrypted", "Encrypted"}, {"compressed", "Compressed"},
            {"yes", "Yes"}, {"no", "No"}, {"op_moved", "Moved"}, {"op_renamed", "Renamed"},
            {"op_added", "Added"}, {"op_deleted", "Deleted"}, {"op_modified", "Modified"},
            {"op_scanned", "Scanned"}, {"basic_colors", "Basic Colors"}, {"custom_colors", "Custom Colors"},
            {"pick_screen_color", "Pick Screen Color"}, {"add_to_custom_colors", "Add to Custom Colors"},
            {"hue", "Hue"}, {"sat", "Saturation"}, {"val", "Value"}, {"red", "Red"},
            {"green", "Green"}, {"blue", "Blue"}, {"html", "HTML"}, {"ok", "OK"},
            {"cancel", "Cancel"}, {"advanced_filters", "Advanced Filters"},
            {"advanced_color_picker", "Advanced Color Picker"}, {"select_color", "Select Color"},
            {"current", "Current"}, {"new", "New"}, {"basics", "Basic"}, {"pastels", "Pastels"},
            {"vibrant", "Vibrant"},
            {"colors_applied_success", "Colors applied to all columns successfully!"},
            {"reset_column_color_confirm", "Do you want to restore column colors to default values?"},
            {"column_colors_reset_success", "Column colors restored successfully!"},
            {"reset_colors_confirm", "Do you want to restore all colors to default values?"},
            {"colors_reset_success", "Colors restored successfully!"},
            {"translating_table", "Aguarde, tradução em andamento..."},
            {"translation_complete", "Tradução concluída!"},
            {"language_change_performance_warning", "Warning: Changing the language will reload translations across the entire UI and may temporarily impact performance until completion. If monitoring is running, this operation can cause missed events or temporary freezes. It is strongly recommended to change the language before starting monitoring. Do you want to proceed?"}
        };
        return fallbacks.value(key, key);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter texto de fallback", true);
        return key;
    }
}

}
