#include "gst_04_GraficoTimeline.h"

#include <algorithm>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSet>
#include <QVariantMap>

#include "source/utils/LogManager.h"

namespace source::data {

namespace {
QDateTime parse_timestamp_value(const QVariant& value) {
    if (value.canConvert<QDateTime>()) {
        const QDateTime dt = value.toDateTime();
        if (dt.isValid()) return dt;
    }

    const QString text = value.toString().trimmed();
    if (text.isEmpty()) return {};

    QDateTime dt = QDateTime::fromString(text, Qt::ISODate);
    if (dt.isValid()) return dt;
    dt = QDateTime::fromString(text, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    if (dt.isValid()) return dt;
    dt = QDateTime::fromString(text, QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
    return dt;
}
}  // namespace

QVariant GraficoTimeline::gerar() {
    try {
        const QString titulo = obter_texto("timeline", QStringLiteral("Linha do Tempo"));
        constexpr int kMaxPontos = 12000;

        const QString conn_name = QStringLiteral("timeline_conn_%1_%2")
                                      .arg(reinterpret_cast<quintptr>(this))
                                      .arg(QDateTime::currentMSecsSinceEpoch());
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name);
        db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=5000"));
        db.setDatabaseName(db_path_);

        if (!db.open()) {
            source::utils::LogManager::get_logger().error(
                QStringLiteral("Erro SQL ao abrir base para timeline: %1").arg(db.lastError().text()), true);
            QSqlDatabase::removeDatabase(conn_name);
            return _criar_grafico_sem_dados(titulo);
        }

        QSqlQuery pragma(db);
        pragma.exec(QStringLiteral("PRAGMA query_only=ON"));
        pragma.exec(QStringLiteral("PRAGMA temp_store=MEMORY"));
        pragma.exec(QStringLiteral("PRAGMA cache_size=-65536"));

        QSqlQuery query(db);
        query.setForwardOnly(true);
        if (!query.exec(QStringLiteral(
                "SELECT tipo_operacao, timestamp "
                "FROM monitoramento "
                "WHERE timestamp IS NOT NULL"))) {
            source::utils::LogManager::get_logger().error(
                QStringLiteral("Erro SQL ao consultar timeline: %1").arg(query.lastError().text()), true);
            db.close();
            QSqlDatabase::removeDatabase(conn_name);
            return _criar_grafico_sem_dados(titulo);
        }

        struct PontoTimeline {
            QDateTime timestamp;
            QString tipo_operacao;
        };

        QVector<PontoTimeline> amostra;
        amostra.reserve(qMin(2048, kMaxPontos));
        QSet<QString> operacoes_usadas;

        const QHash<QString, QString> mapa_ops = _obter_mapeamento_operacoes();
        int stride = 1;
        qint64 total_validos = 0;

        while (query.next()) {
            const QDateTime dt = parse_timestamp_value(query.value(1));
            if (!dt.isValid()) continue;

            QString op = query.value(0).toString().trimmed();
            if (op.isEmpty()) {
                op = obter_texto("unknown", QStringLiteral("Desconhecido"));
            }
            op = mapa_ops.value(op, op);
            operacoes_usadas.insert(op);

            if ((total_validos % stride) == 0) {
                amostra.append({dt, op});
                if (amostra.size() > kMaxPontos) {
                    QVector<PontoTimeline> reduzido;
                    reduzido.reserve((amostra.size() + 1) / 2);
                    for (int i = 0; i < amostra.size(); i += 2) {
                        reduzido.append(amostra.at(i));
                    }
                    amostra = std::move(reduzido);
                    stride *= 2;
                }
            }
            ++total_validos;
        }

        db.close();
        QSqlDatabase::removeDatabase(conn_name);

        if (amostra.isEmpty()) return _criar_grafico_sem_dados(titulo);
        std::sort(amostra.begin(), amostra.end(), [](const PontoTimeline& a, const PontoTimeline& b) {
            return a.timestamp < b.timestamp;
        });

        QVariantList pontos;
        pontos.reserve(amostra.size());
        QVariantMap op_colors;
        for (const PontoTimeline& ponto : amostra) {
            QVariantMap p;
            p.insert(QStringLiteral("timestamp"), ponto.timestamp.toString(Qt::ISODate));
            p.insert(QStringLiteral("tipo_operacao"), ponto.tipo_operacao);
            pontos.append(p);
        }
        for (const QString& op : operacoes_usadas) {
            op_colors.insert(op, cores_operacoes_.value(op, QStringLiteral("#333333")));
        }

        QVariantMap out;
        out.insert(QStringLiteral("kind"), QStringLiteral("timeline"));
        out.insert(QStringLiteral("title"), titulo);
        out.insert(QStringLiteral("points"), pontos);
        out.insert(QStringLiteral("op_colors"), op_colors);
        out.insert(QStringLiteral("source_points"), total_validos);
        out.insert(QStringLiteral("display_points"), pontos.size());
        out.insert(QStringLiteral("sample_stride"), stride);
        out.insert(QStringLiteral("fast_point_cloud"), true);
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar gráfico timeline", true);
        return _criar_grafico_sem_dados(obter_texto("timeline", QStringLiteral("Linha do Tempo")));
    }
}

}  // namespace source::data
