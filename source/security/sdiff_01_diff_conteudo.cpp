#include "sdiff_01_diff_conteudo.h"

#include <QFile>

namespace source::security {
namespace {

QString load_text(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }
    const QString text = QString::fromUtf8(file.readAll());
    file.close();
    return text;
}

QString resolve_snapshot_text(const QVariantMap& evento,
                              const QString& text_key,
                              const QString& path_key) {
    const QString inline_text = evento.value(text_key).toString();
    if (!inline_text.isEmpty()) {
        return inline_text;
    }

    const QString path = evento.value(path_key).toString();
    if (path.isEmpty()) {
        return QString();
    }
    return load_text(path);
}

}  // namespace

QVariantMap DiffConteudo::summarize(const QVariantMap& evento, const QVariantMap&) const {
    QVariantMap out;
    const QString previous_path = evento.value(QStringLiteral("_baseline_snapshot_prev")).toString();
    const QString current_path = evento.value(QStringLiteral("_baseline_snapshot_current")).toString();
    const QString previous_text = resolve_snapshot_text(
        evento,
        QStringLiteral("_baseline_snapshot_prev_text"),
        QStringLiteral("_baseline_snapshot_prev"));
    const QString current_text = resolve_snapshot_text(
        evento,
        QStringLiteral("_baseline_snapshot_current_text"),
        QStringLiteral("_baseline_snapshot_current"));

    if (previous_path.isEmpty() && current_path.isEmpty() &&
        previous_text.isEmpty() && current_text.isEmpty()) {
        return out;
    }
    const QStringList old_lines = previous_text.split(QLatin1Char('\n'));
    const QStringList new_lines = current_text.split(QLatin1Char('\n'));

    int prefix = 0;
    while (prefix < old_lines.size() && prefix < new_lines.size() && old_lines.at(prefix) == new_lines.at(prefix)) {
        ++prefix;
    }

    int suffix = 0;
    while ((suffix + prefix) < old_lines.size() && (suffix + prefix) < new_lines.size() &&
           old_lines.at(old_lines.size() - 1 - suffix) == new_lines.at(new_lines.size() - 1 - suffix)) {
        ++suffix;
    }

    const int removed = qMax(0, old_lines.size() - prefix - suffix);
    const int added = qMax(0, new_lines.size() - prefix - suffix);

    if (added == 0 && removed == 0) {
        out.insert(QStringLiteral("diff_resumo"), QStringLiteral("sem_alteracao_textual"));
    } else {
        out.insert(QStringLiteral("diff_resumo"),
                   QStringLiteral("security_diff_lines_changed|%1|%2").arg(added).arg(removed));
    }
    return out;
}

}  // namespace source::security
