#include "gbank_19_inserir_evento_movido.h"

#include <QDateTime>
#include <QFileInfo>
#include <QSet>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUuid>

#include "source/security/secore_02_SecurityManager.h"
#include "source/security/secui_00_helpers.h"
#include "source/security/secore_03_evento_campos.h"
#include "source/services/GerenciamentoBaseEvento/gbank_00_operacoes.h"
#include "source/services/GerenciamentoMetadados/gmet_19_GetTipoArquivo.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
QString conn_name(const QString& db) {
    return QStringLiteral("baseevento_ins_mov_%1_%2")
        .arg(reinterpret_cast<quintptr>(db.constData()))
        .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
}

bool has_text_value(const QVariant& value) {
    return !value.toString().trimmed().isEmpty();
}

bool has_positive_numeric_value(const QVariant& value) {
    bool ok = false;
    const double number = value.toString().replace(',', '.').toDouble(&ok);
    return ok && number > 0.0;
}

bool is_zero_like_value(const QVariant& value) {
    const QString text = value.toString().trimmed();
    if (text.isEmpty()) {
        return false;
    }

    bool ok = false;
    const double parsed = QString(text).replace(',', '.').toDouble(&ok);
    return ok && qFuzzyIsNull(parsed);
}

bool has_meaningful_moved_metadata(const QVariantMap& evento) {
    static const QStringList kSignalKeys = {
        QStringLiteral("size_b"),
        QStringLiteral("size_kb"),
        QStringLiteral("size_mb"),
        QStringLiteral("size_gb"),
        QStringLiteral("size_tb"),
        QStringLiteral("mime"),
        QStringLiteral("dimensoes"),
        QStringLiteral("duracao"),
        QStringLiteral("taxa_bits"),
        QStringLiteral("paginas"),
        QStringLiteral("linhas"),
        QStringLiteral("palavras"),
        QStringLiteral("arquivos")
    };

    for (const QString& key : kSignalKeys) {
        const QVariant value = evento.value(key);
        if (has_positive_numeric_value(value) || has_text_value(value)) {
            return true;
        }
    }
    return false;
}

void fill_missing_moved_metadata(QVariantMap& destino, const QVariantMap& origem) {
    static const QSet<QString> kNumericKeys = {
        QStringLiteral("size_b"), QStringLiteral("size_kb"), QStringLiteral("size_mb"),
        QStringLiteral("size_gb"), QStringLiteral("size_tb"), QStringLiteral("arquivos"),
        QStringLiteral("paginas"), QStringLiteral("linhas"), QStringLiteral("palavras"),
        QStringLiteral("paginas_estimadas"), QStringLiteral("linhas_codigo"),
        QStringLiteral("total_linhas"), QStringLiteral("slides_estimados"),
        QStringLiteral("unzipped_b"), QStringLiteral("unzipped_kb"), QStringLiteral("unzipped_mb"),
        QStringLiteral("unzipped_gb"), QStringLiteral("unzipped_tb"), QStringLiteral("slides"),
        QStringLiteral("binary_file_b"), QStringLiteral("binary_file_kb"),
        QStringLiteral("binary_file_mb"), QStringLiteral("binary_file_gb"),
        QStringLiteral("binary_file_tb"), QStringLiteral("planilhas"),
        QStringLiteral("colunas"), QStringLiteral("registros"), QStringLiteral("tabelas")
    };
    static const QSet<QString> kTextKeys = {
        QStringLiteral("data_criacao"), QStringLiteral("data_modificacao"), QStringLiteral("data_acesso"),
        QStringLiteral("tipo"), QStringLiteral("mime"), QStringLiteral("atributos"), QStringLiteral("autor"),
        QStringLiteral("dimensoes"), QStringLiteral("duracao"), QStringLiteral("taxa_bits"),
        QStringLiteral("protegido")
    };

    for (auto it = origem.constBegin(); it != origem.constEnd(); ++it) {
        const QString key = it.key();
        const QVariant candidate = it.value();
        if (key == QStringLiteral("tipo_operacao") ||
            key == QStringLiteral("nome") ||
            key == QStringLiteral("dir_anterior") ||
            key == QStringLiteral("dir_atual") ||
            key == QStringLiteral("timestamp")) {
            continue;
        }

        const QVariant current = destino.value(key);
        if (kNumericKeys.contains(key)) {
            if ((current.isNull() || current.toString().trimmed().isEmpty() || is_zero_like_value(current)) &&
                (has_positive_numeric_value(candidate) || has_text_value(candidate))) {
                destino.insert(key, candidate);
            }
            continue;
        }

        if (kTextKeys.contains(key)) {
            if (!has_text_value(current) && has_text_value(candidate)) {
                destino.insert(key, candidate);
            }
            continue;
        }

        if (!destino.contains(key) || (!has_text_value(current) && has_text_value(candidate))) {
            destino.insert(key, candidate);
        }
    }
}
}

namespace source::services::GerenciamentoBaseEvento {

QVariantMap preparar_evento_movido(source::services::BaseEvento* self, const QVariantMap& evento) {
    if (!self || !self->observador || evento.isEmpty()) {
        return evento;
    }

    QVariantMap preparado = evento;
    const QString nomeBase = QFileInfo(evento.value(QStringLiteral("nome")).toString().trimmed()).fileName().trimmed();
    const QString tipoPreferencial =
        source::services::GerenciamentoMetadados::extensao_tipo_preferencial(nomeBase);
    const QString dirAnterior = evento.value(QStringLiteral("dir_anterior")).toString().trimmed();
    const QString dirAtual = evento.value(QStringLiteral("dir_atual")).toString().trimmed();
    const QString timestampEvento =
        evento.value(QStringLiteral("timestamp")).toString().trimmed().isEmpty()
            ? QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"))
            : evento.value(QStringLiteral("timestamp")).toString().trimmed();

    preparado.insert(QStringLiteral("tipo_operacao"), QStringLiteral("op_moved"));
    preparado.insert(QStringLiteral("nome"), nomeBase);
    preparado.insert(QStringLiteral("dir_anterior"), dirAnterior);
    preparado.insert(QStringLiteral("dir_atual"), dirAtual);
    preparado.insert(QStringLiteral("timestamp"), timestampEvento);

    const QVariantMap reconstruido =
        self->_criar_evento_padrao(QStringLiteral("op_moved"), nomeBase, dirAnterior, dirAtual);
    fill_missing_moved_metadata(preparado, reconstruido);

    if (!has_meaningful_moved_metadata(preparado)) {
        if (!dirAtual.isEmpty()) {
            fill_missing_moved_metadata(preparado, self->obter_metadados_arquivo_excluido(nomeBase, dirAtual));
        }
        if (!has_meaningful_moved_metadata(preparado) && !dirAnterior.isEmpty()) {
            fill_missing_moved_metadata(preparado, self->obter_metadados_arquivo_excluido(nomeBase, dirAnterior));
        }
        if (!has_meaningful_moved_metadata(preparado)) {
            fill_missing_moved_metadata(preparado, self->obter_metadados_arquivo_excluido(nomeBase, QString()));
        }
    }

    preparado.insert(QStringLiteral("tipo_operacao"), QStringLiteral("op_moved"));
    preparado.insert(QStringLiteral("nome"), nomeBase);
    preparado.insert(QStringLiteral("dir_anterior"), dirAnterior);
    preparado.insert(QStringLiteral("dir_atual"), dirAtual);
    preparado.insert(QStringLiteral("timestamp"), timestampEvento);
    if (!tipoPreferencial.isEmpty()) {
        preparado.insert(QStringLiteral("tipo"), tipoPreferencial);
    }
    return preparado;
}

bool _inserir_evento_movido_query(source::services::BaseEvento* self, QSqlQuery& cursor, const QVariantMap& evento) {
    if (!self || !self->observador) return false;

    const QString opMoved = QStringLiteral("op_moved");
    if (!operacao_corresponde(evento.value("tipo_operacao").toString(),
                              QStringLiteral("op_moved"),
                              self->observador->loc)) {
        return false;
    }

    try {
        const QStringList colunas = source::security::core::colunas_evento_monitoramento();
        QString ph;
        for (int i = 0; i < colunas.size(); ++i) ph += (i ? ",?" : "?");

        QVariantMap ev = preparar_evento_movido(self, evento);
        ev["tipo_operacao"] = opMoved;
        if (!ev.contains("timestamp")) ev["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
        if (self->observador != nullptr && self->observador->interface != nullptr) {
            if (auto* security_manager = source::security::ui::get_security_manager(
                    self->observador->interface);
                security_manager != nullptr) {
                ev = security_manager->enrich_event(ev);
            }
        }
        source::security::core::garantir_campos_evento_monitoramento(ev);

        cursor.prepare(QString("INSERT INTO movido (%1) VALUES (%2)").arg(colunas.join(','), ph));
        for (const QString& c : colunas) {
            if (c == "tipo_operacao") cursor.addBindValue(opMoved);
            else cursor.addBindValue(ev.value(c, ""));
        }
        if (!cursor.exec()) {
            return false;
        }

        cursor.prepare(QString("INSERT INTO monitoramento (%1) VALUES (%2)").arg(colunas.join(','), ph));
        for (const QString& c : colunas) {
            if (c == "tipo_operacao") cursor.addBindValue(opMoved);
            else cursor.addBindValue(ev.value(c, ""));
        }
        return cursor.exec();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inserir evento movido", true);
        return false;
    }
}

bool _inserir_evento_movido(source::services::BaseEvento* self, const QVariantMap& evento) {
    if (!self) return false;

    QSqlDatabase db;
    QString connName;
    try {
        connName = conn_name(self->db_path + evento.value("nome").toString());
        db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName(self->db_path);
        if (!db.open()) {
            QSqlDatabase::removeDatabase(connName);
            return false;
        }

        db.transaction();
        QSqlQuery cursor(db);
        const bool ok = _inserir_evento_movido_query(self, cursor, evento);
        if (ok) {
            db.commit();
        } else {
            db.rollback();
        }

        db.close();
        QSqlDatabase::removeDatabase(connName);
        return ok;
    } catch (...) {
        if (db.isValid() && db.isOpen()) {
            db.rollback();
            db.close();
        }
        if (!connName.isEmpty()) {
            QSqlDatabase::removeDatabase(connName);
        }
        source::utils::LogManager::get_logger().error("Erro ao inserir evento movido", true);
        return false;
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
