#include "sess_01_correlacao_sessoes.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QFileInfo>
#include <QHash>
#include <QMutex>
#include <QMutexLocker>
#include <QSet>
#include <QSqlQuery>

#include "source/security/secore_01_postgres_utils.h"
#include "source/security/secore_05_SecurityDbWriter.h"

namespace source::security {
namespace {

struct SessionState {
    QString id;
    QString label;
    qint64 started_ms = 0;
    qint64 updated_ms = 0;
    qint64 last_persist_ms = 0;
    int total_events = 0;
    int added = 0;
    int modified = 0;
    int deleted = 0;
    QSet<QString> directories;
};

QHash<QString, SessionState>& session_cache() {
    static QHash<QString, SessionState> cache;
    return cache;
}

QMutex& session_cache_lock() {
    static QMutex mutex;
    return mutex;
}

qint64 timestamp_ms(const QVariantMap& evento) {
    QDateTime dt = QDateTime::fromString(evento.value(QStringLiteral("timestamp")).toString(),
                                         QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
    if (!dt.isValid()) {
        dt = QDateTime::currentDateTime();
    }
    return dt.toMSecsSinceEpoch();
}

QString classify_label(const SessionState& state, const QVariantMap& evento) {
    const QString ext = QFileInfo(evento.value(QStringLiteral("nome")).toString()).suffix().toLower();
    if (state.deleted >= 10 && state.deleted >= state.total_events / 2) {
        return QStringLiteral("limpeza_automatica");
    }
    if (state.added + state.modified >= 8 &&
        (ext == QStringLiteral("cpp") || ext == QStringLiteral("h") || ext == QStringLiteral("hpp") ||
         ext == QStringLiteral("py") || ext == QStringLiteral("json") || ext == QStringLiteral("xml"))) {
        return QStringLiteral("build_refatoracao");
    }
    if (state.added >= 8 &&
        (ext == QStringLiteral("exe") || ext == QStringLiteral("dll") || ext == QStringLiteral("msi") ||
         ext == QStringLiteral("zip") || ext == QStringLiteral("7z"))) {
        return QStringLiteral("instalacao_ou_extracao");
    }
    return QStringLiteral("sessao_monitoramento");
}

void persist_session(const QString& profile_id, const SessionState& state) {
    QVariantMap row;
    row.insert(QStringLiteral("session_id"), state.id);
    row.insert(QStringLiteral("profile_id"), profile_id);
    row.insert(QStringLiteral("label"), state.label);
    row.insert(QStringLiteral("started_at"),
               QDateTime::fromMSecsSinceEpoch(state.started_ms).toString(Qt::ISODateWithMs));
    row.insert(QStringLiteral("updated_at"),
               QDateTime::fromMSecsSinceEpoch(state.updated_ms).toString(Qt::ISODateWithMs));
    row.insert(QStringLiteral("total_eventos"), state.total_events);
    row.insert(QStringLiteral("diretorios"),
               QStringList(state.directories.values()).join(QStringLiteral("; ")));
    SecurityDbWriter::instance().enqueue_session_upsert(row);
}

}  // namespace

void CorrelacaoSessoes::ensure_schema(const QString& db_path) const {
    Q_UNUSED(db_path)

    QSqlDatabase db = source::security::pg::open_database(QStringLiteral("sessions_schema"));
    if (!db.isValid() || !db.isOpen()) {
        return;
    }

    QSqlQuery query(db);
    query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS security_sessions ("
        "session_id TEXT PRIMARY KEY, "
        "profile_id TEXT, "
        "label TEXT, "
        "started_at TEXT, "
        "updated_at TEXT, "
        "total_eventos INTEGER, "
        "diretorios TEXT)"));
    query.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_security_sessions_profile_updated "
        "ON security_sessions (profile_id, updated_at DESC)"));
    query.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_security_sessions_updated "
        "ON security_sessions (updated_at DESC)"));
}

QVariantMap CorrelacaoSessoes::correlate(const QVariantMap& evento, const QVariantMap& contexto) {
    QVariantMap out;
    const QString profile_id = contexto.value(QStringLiteral("monitor_profile_id")).toString();
    if (profile_id.isEmpty()) {
        return out;
    }

    const qint64 now_ms = timestamp_ms(evento);
    QMutexLocker locker(&session_cache_lock());
    SessionState& state = session_cache()[profile_id];
    const bool expired = (state.updated_ms > 0 && (now_ms - state.updated_ms) > 120000);
    if (state.id.isEmpty() || expired) {
        const QByteArray digest =
            QCryptographicHash::hash(QString::number(now_ms).toUtf8(), QCryptographicHash::Sha1).toHex();
        state = SessionState();
        state.id = QStringLiteral("session_%1").arg(QString::fromLatin1(digest.left(12)));
        state.started_ms = now_ms;
    }

    const QString op = evento.value(QStringLiteral("tipo_operacao")).toString().toLower();
    const QString label_anterior = state.label;
    state.updated_ms = now_ms;
    state.total_events += 1;
    if (op.contains(QStringLiteral("added")) || op.contains(QStringLiteral("adicionado"))) {
        state.added += 1;
    }
    if (op.contains(QStringLiteral("modified")) || op.contains(QStringLiteral("modificado"))) {
        state.modified += 1;
    }
    if (op.contains(QStringLiteral("deleted")) || op.contains(QStringLiteral("excluido")) ||
        op.contains(QStringLiteral("excluído"))) {
        state.deleted += 1;
    }
    const QString dir = evento.value(QStringLiteral("dir_atual")).toString().isEmpty()
                            ? evento.value(QStringLiteral("dir_anterior")).toString()
                            : evento.value(QStringLiteral("dir_atual")).toString();
    if (!dir.isEmpty()) {
        state.directories.insert(dir);
    }
    state.label = classify_label(state, evento);

    const bool deve_persistir =
        (state.total_events <= 1) ||
        (state.label != label_anterior) ||
        ((now_ms - state.last_persist_ms) >= 2000) ||
        ((state.total_events % 25) == 0);

    if (deve_persistir) {
        persist_session(profile_id, state);
        state.last_persist_ms = now_ms;
    }

    out.insert(QStringLiteral("session_id"), state.id);
    out.insert(QStringLiteral("session_label"), state.label);
    return out;
}

void CorrelacaoSessoes::clear_runtime_cache() const {
    QMutexLocker locker(&session_cache_lock());
    session_cache().clear();
}

}  // namespace source::security
