#include "agre_01_motor_regras.h"

#include <QDateTime>
#include <QFileInfo>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMutex>
#include <QMutexLocker>
#include <QSet>
#include <QSqlQuery>

#include "source/security/secore_01_postgres_utils.h"
#include "source/security/secore_05_SecurityDbWriter.h"

namespace source::security {
namespace {

QHash<QString, QList<qint64>>& delete_window() {
    static QHash<QString, QList<qint64>> cache;
    return cache;
}

QMutex& delete_window_lock() {
    static QMutex mutex;
    return mutex;
}

bool is_sensitive_path(const QString& path) {
    const QString lowered = path.toLower();
    return lowered.contains(QStringLiteral("startup")) ||
           lowered.contains(QStringLiteral("system32")) ||
           lowered.contains(QStringLiteral("programdata")) ||
           lowered.contains(QStringLiteral("appdata")) ||
           lowered.contains(QStringLiteral("windows\\tasks"));
}

bool is_script_or_executable(const QString& name) {
    static const QSet<QString> exts = {
        QStringLiteral("exe"), QStringLiteral("dll"), QStringLiteral("msi"), QStringLiteral("ps1"),
        QStringLiteral("bat"), QStringLiteral("cmd"), QStringLiteral("vbs"), QStringLiteral("js"),
    };
    return exts.contains(QFileInfo(name).suffix().toLower());
}

qint64 timestamp_ms(const QVariantMap& evento) {
    QDateTime dt = QDateTime::fromString(evento.value(QStringLiteral("timestamp")).toString(),
                                         QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
    if (!dt.isValid()) {
        dt = QDateTime::currentDateTime();
    }
    return dt.toMSecsSinceEpoch();
}

QString join_rules(const QVariantMap& evento) {
    const QString compact = evento.value(QStringLiteral("alerta_regras")).toString();
    if (compact.isEmpty()) {
        return QString();
    }

    const QJsonDocument doc = QJsonDocument::fromJson(compact.toUtf8());
    if (!doc.isArray()) {
        return compact;
    }

    QStringList rules;
    for (const QJsonValue& value : doc.array()) {
        const QString rule = value.toString().trimmed();
        if (!rule.isEmpty()) {
            rules.append(rule);
        }
    }
    return rules.join(QStringLiteral("; "));
}

}  // namespace

void MotorRegras::ensure_schema(const QString& db_path) const {
    Q_UNUSED(db_path)

    QSqlDatabase db = source::security::pg::open_database(QStringLiteral("alerts_schema"));
    if (!db.isValid() || !db.isOpen()) {
        return;
    }

    QSqlQuery query(db);
    query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS security_alerts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created_at TEXT, "
        "profile_id TEXT, "
        "nome TEXT, "
        "caminho TEXT, "
        "risk_level TEXT, "
        "rules TEXT, "
        "summary TEXT, "
        "session_id TEXT, "
        "evidence_id TEXT)"));
    query.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_security_alerts_profile_created "
        "ON security_alerts (profile_id, created_at DESC)"));
    query.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_security_alerts_created "
        "ON security_alerts (created_at DESC)"));
}

QVariantMap MotorRegras::evaluate(const QVariantMap& evento, const QVariantMap& contexto) {
    QVariantMap out;
    const QString profile_id = contexto.value(QStringLiteral("monitor_profile_id")).toString();
    const QString path = evento.value(QStringLiteral("dir_atual")).toString().isEmpty()
                             ? evento.value(QStringLiteral("dir_anterior")).toString()
                             : evento.value(QStringLiteral("dir_atual")).toString();
    const QString op = evento.value(QStringLiteral("tipo_operacao")).toString().toLower();

    QStringList rules;
    if (is_script_or_executable(evento.value(QStringLiteral("nome")).toString()) &&
        is_sensitive_path(path) &&
        (op.contains(QStringLiteral("added")) || op.contains(QStringLiteral("adicionado")))) {
        rules.append(QStringLiteral("binario_novo_em_pasta_sensivel"));
    }

    if (op.contains(QStringLiteral("deleted")) || op.contains(QStringLiteral("excluido")) ||
        op.contains(QStringLiteral("excluído"))) {
        QMutexLocker locker(&delete_window_lock());
        QList<qint64>& deletions = delete_window()[profile_id];
        const qint64 now_ms = timestamp_ms(evento);
        deletions.append(now_ms);
        while (!deletions.isEmpty() && (now_ms - deletions.front()) > 60000) {
            deletions.removeFirst();
        }
        if (deletions.size() >= 100) {
            rules.append(QStringLiteral("explosao_exclusoes_em_1_minuto"));
        }
    }

    const QString signature_state = evento.value(QStringLiteral("signature_state")).toString().toLower();
    if ((signature_state == QStringLiteral("não") || signature_state == QStringLiteral("no")) &&
        is_script_or_executable(evento.value(QStringLiteral("nome")).toString())) {
        rules.append(QStringLiteral("executavel_ou_script_sem_assinatura"));
    }

    if (path.toLower().contains(QStringLiteral("startup")) &&
        evento.value(QStringLiteral("script_risk")).toString() == QStringLiteral("high")) {
        rules.append(QStringLiteral("script_em_startup"));
    }

    if (evento.value(QStringLiteral("integrity_flags")).toString().contains(QStringLiteral("hash_changed_without_mtime"))) {
        rules.append(QStringLiteral("alteracao_sem_mudanca_de_timestamp"));
    }

    QString level = QStringLiteral("none");
    if (!rules.isEmpty()) {
        level = rules.size() >= 2 ? QStringLiteral("high") : QStringLiteral("medium");
    }
    const QString summary = rules.mid(0, 3).join(QStringLiteral("; "));

    out.insert(QStringLiteral("alerta_regras"),
               QString::fromUtf8(QJsonDocument(QJsonArray::fromStringList(rules))
                                     .toJson(QJsonDocument::Compact)));
    out.insert(QStringLiteral("alerta_resumo"), summary);
    out.insert(QStringLiteral("alerta_nivel"), level);
    return out;
}

void MotorRegras::persist_if_needed(const QVariantMap& evento, const QVariantMap& contexto) const {
    const QString profile_id = contexto.value(QStringLiteral("monitor_profile_id")).toString();
    if (profile_id.isEmpty()) {
        return;
    }

    const QString summary = evento.value(QStringLiteral("alerta_resumo")).toString().trimmed();
    const QString level = evento.value(QStringLiteral("alerta_nivel")).toString().trimmed();
    const QString rules = join_rules(evento);
    if (summary.isEmpty() || level.isEmpty() || level == QStringLiteral("none")) {
        return;
    }

    QVariantMap row;
    row.insert(QStringLiteral("created_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
    row.insert(QStringLiteral("profile_id"), profile_id);
    row.insert(QStringLiteral("nome"), evento.value(QStringLiteral("nome")).toString());
    row.insert(QStringLiteral("caminho"),
               evento.value(QStringLiteral("dir_atual")).toString().isEmpty()
                   ? evento.value(QStringLiteral("dir_anterior")).toString()
                   : evento.value(QStringLiteral("dir_atual")).toString());
    row.insert(QStringLiteral("risk_level"), level);
    row.insert(QStringLiteral("rules"), rules);
    row.insert(QStringLiteral("summary"), summary);
    row.insert(QStringLiteral("session_id"), evento.value(QStringLiteral("session_id")).toString());
    row.insert(QStringLiteral("evidence_id"), evento.value(QStringLiteral("evidence_id")).toString());
    SecurityDbWriter::instance().enqueue_alert_insert(row);
}

void MotorRegras::clear_runtime_cache() const {
    QMutexLocker locker(&delete_window_lock());
    delete_window().clear();
}

}  // namespace source::security
