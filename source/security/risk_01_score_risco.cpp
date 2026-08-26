#include "risk_01_score_risco.h"

#include <QDateTime>
#include <QFileInfo>
#include <QSet>

namespace source::security {
namespace {

bool is_sensitive_path(const QString& path) {
    const QString lowered = path.toLower();
    return lowered.contains(QStringLiteral("startup")) ||
           lowered.contains(QStringLiteral("system32")) ||
           lowered.contains(QStringLiteral("programdata")) ||
           lowered.contains(QStringLiteral("appdata"));
}

bool is_script_or_executable(const QString& name) {
    static const QSet<QString> exts = {
        QStringLiteral("exe"), QStringLiteral("dll"), QStringLiteral("msi"), QStringLiteral("ps1"),
        QStringLiteral("bat"), QStringLiteral("cmd"), QStringLiteral("vbs"), QStringLiteral("js"),
    };
    return exts.contains(QFileInfo(name).suffix().toLower());
}

}  // namespace

QVariantMap ScoreRisco::score(const QVariantMap& evento, const QVariantMap&) const {
    QVariantMap out;
    int risk = 0;
    QStringList reasons;

    const QString path = evento.value(QStringLiteral("dir_atual")).toString().isEmpty()
                             ? evento.value(QStringLiteral("dir_anterior")).toString()
                             : evento.value(QStringLiteral("dir_atual")).toString();
    const QString op = evento.value(QStringLiteral("tipo_operacao")).toString().toLower();
    const QString name = evento.value(QStringLiteral("nome")).toString();

    if (is_script_or_executable(name)) {
        risk += 20;
        reasons.append(QStringLiteral("executavel_ou_script"));
    }
    if (is_sensitive_path(path)) {
        risk += 15;
        reasons.append(QStringLiteral("caminho_sensivel"));
    }
    if (op.contains(QStringLiteral("deleted")) || op.contains(QStringLiteral("excluido")) ||
        op.contains(QStringLiteral("excluído"))) {
        risk += 15;
        reasons.append(QStringLiteral("operacao_exclusao"));
    } else if (op.contains(QStringLiteral("modified")) || op.contains(QStringLiteral("modificado"))) {
        risk += 10;
        reasons.append(QStringLiteral("operacao_modificacao"));
    }
    if (!evento.value(QStringLiteral("alerta_resumo")).toString().isEmpty()) {
        risk += 30;
        reasons.append(QStringLiteral("motor_regras"));
    }
    const QString signature_state = evento.value(QStringLiteral("signature_state")).toString().toLower();
    if (signature_state == QStringLiteral("não") || signature_state == QStringLiteral("no")) {
        risk += 15;
        reasons.append(QStringLiteral("sem_assinatura"));
    }
    if (evento.value(QStringLiteral("integrity_flags")).toString().contains(QStringLiteral("hash_changed_without_mtime"))) {
        risk += 20;
        reasons.append(QStringLiteral("hash_sem_timestamp"));
    }
    if (evento.value(QStringLiteral("script_risk")).toString() == QStringLiteral("high")) {
        risk += 15;
        reasons.append(QStringLiteral("script_risk_high"));
    }

    QDateTime dt = QDateTime::fromString(evento.value(QStringLiteral("timestamp")).toString(),
                                         QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
    if (!dt.isValid()) {
        dt = QDateTime::currentDateTime();
    }
    const int hour = dt.time().hour();
    if (hour < 6 || hour >= 20) {
        risk += 10;
        reasons.append(QStringLiteral("fora_do_horario"));
    }

    if (risk > 100) {
        risk = 100;
    }

    QString level = QStringLiteral("low");
    if (risk >= 70) {
        level = QStringLiteral("critical");
    } else if (risk >= 45) {
        level = QStringLiteral("high");
    } else if (risk >= 20) {
        level = QStringLiteral("medium");
    }

    out.insert(QStringLiteral("risk_score"), risk);
    out.insert(QStringLiteral("risk_level"), level);
    out.insert(QStringLiteral("risk_reasons"), reasons.join(QStringLiteral("; ")));
    return out;
}

}  // namespace source::security
