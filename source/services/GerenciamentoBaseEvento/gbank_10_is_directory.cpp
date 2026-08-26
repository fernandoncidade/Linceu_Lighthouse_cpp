#include "gbank_10_is_directory.h"

#include <QFileInfo>

#include "source/services/ob_02_BaseEvento.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoBaseEvento {

bool is_directory(source::services::BaseEvento* self, const QString& path) {
    Q_UNUSED(self)
    try {
        return QFileInfo(path).isDir();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao verificar se é diretório", true);
        return false;
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
