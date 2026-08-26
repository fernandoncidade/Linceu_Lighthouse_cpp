#include "attr_01_atribuicao_forense.h"

#include <QFileInfo>

namespace source::security {

QVariantMap AtribuicaoForense::attribute(const QVariantMap& evento, const QVariantMap&) const {
    QVariantMap out;
    const QString path = evento.value(QStringLiteral("dir_atual")).toString().isEmpty()
                             ? evento.value(QStringLiteral("dir_anterior")).toString()
                             : evento.value(QStringLiteral("dir_atual")).toString();

    QString actor_user = evento.value(QStringLiteral("autor")).toString().trimmed();
    QString attribution_source = QStringLiteral("metadata_author");
    if (actor_user.isEmpty() && !path.isEmpty()) {
        const QFileInfo info(path);
        actor_user = info.owner();
        attribution_source = QStringLiteral("file_owner");
    }
    if (actor_user.isEmpty()) {
        actor_user = qEnvironmentVariable("USERNAME");
        attribution_source = QStringLiteral("environment_user");
    }

    out.insert(QStringLiteral("actor_user"), actor_user);
    out.insert(QStringLiteral("actor_process"), QStringLiteral("unknown_process"));
    out.insert(QStringLiteral("actor_pid"), 0);
    out.insert(QStringLiteral("attribution_source"), attribution_source);
    return out;
}

}  // namespace source::security
