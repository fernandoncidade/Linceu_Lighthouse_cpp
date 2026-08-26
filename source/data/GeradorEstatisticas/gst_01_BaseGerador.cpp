#include "gst_01_BaseGerador.h"

#include <algorithm>

#include <QDir>
#include <QMetaObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QThread>

#include "source/ui/GerenciamentoMenusUI/gmui_02_GerenciadorCores.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::data {

namespace {
Qt::ConnectionType invoke_connection_type(QObject* object) {
    if (object == nullptr || object->thread() == QThread::currentThread()) {
        return Qt::DirectConnection;
    }
    return Qt::BlockingQueuedConnection;
}
}

BaseGerador::BaseGerador(const QString& db_path, QObject* localizador, QObject* interface_principal)
    : db_path_(db_path),
      loc_(localizador),
      interface_(interface_principal),
      alive_flag_(std::make_shared<std::atomic_bool>(true)) {
    _atualizar_textos_traduzidos();

    try {
        if (auto* loc_tipado = qobject_cast<source::ui::LocalizadorQt*>(loc_);
            loc_tipado != nullptr && loc_tipado->thread() == QThread::currentThread()) {
            QObject::connect(loc_tipado, &source::ui::LocalizadorQt::idioma_alterado, loc_tipado,
                             [alive = alive_flag_, this](const QString&) {
                                 if (!alive || !alive->load()) return;
                                 _atualizar_textos_traduzidos();
                             });
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            QStringLiteral("Erro ao conectar atualização de idioma em BaseGerador"), true);
    }
}

BaseGerador::~BaseGerador() {
    if (alive_flag_) alive_flag_->store(false);
}

QVariant BaseGerador::gerar() {
    return _criar_grafico_sem_dados(obter_texto("no_data_to_plot", QStringLiteral("Sem dados para exibir")));
}

void BaseGerador::atualizar_textos_traduzidos() { _atualizar_textos_traduzidos(); }

QString BaseGerador::_obter_cor_hex_operacao(const QString& op_key, const QString& fallback_hex) const {
    try {
        QObject* gerenciador_cores_obj =
            interface_ ? interface_->property("gerenciador_cores").value<QObject*>() : nullptr;
        if (gerenciador_cores_obj == nullptr && interface_ != nullptr) {
            if (QObject* menus = interface_->property("gerenciador_menus_ui").value<QObject*>()) {
                gerenciador_cores_obj = menus->property("gerenciador_cores").value<QObject*>();
            }
        }

        if (auto* gerenciador_cores =
                qobject_cast<source::ui::GerenciamentoMenusUI::GerenciadorCores*>(gerenciador_cores_obj);
            gerenciador_cores != nullptr && gerenciador_cores->thread() == QThread::currentThread()) {
            const QString cor = gerenciador_cores->obter_cor_hex(op_key);
            if (!cor.trimmed().isEmpty()) return cor;
        }

        if (gerenciador_cores_obj != nullptr) {
            QString cor;
            QMetaObject::invokeMethod(gerenciador_cores_obj, "obter_cor_hex",
                                      invoke_connection_type(gerenciador_cores_obj),
                                      Q_RETURN_ARG(QString, cor), Q_ARG(QString, op_key));
            if (!cor.trimmed().isEmpty()) return cor;
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            QStringLiteral("Erro ao obter cor da operação '%1'").arg(op_key), true);
    }
    return fallback_hex;
}

void BaseGerador::_atualizar_textos_traduzidos() {
    try {
        cores_operacoes_.clear();
        const auto registrar_cor = [this](const QString& chave, const QString& fallback_texto,
                                          const QString& fallback_cor) {
            const QString texto_traduzido = obter_texto(chave, fallback_texto);
            const QString cor = _obter_cor_hex_operacao(chave, fallback_cor);
            cores_operacoes_.insert(chave, cor);
            cores_operacoes_.insert(texto_traduzido, cor);
        };

        registrar_cor(QStringLiteral("op_renamed"), QStringLiteral("Renomeado"), QStringLiteral("#00ff00"));
        registrar_cor(QStringLiteral("op_added"), QStringLiteral("Adicionado"), QStringLiteral("#0000ff"));
        registrar_cor(QStringLiteral("op_deleted"), QStringLiteral("Excluído"), QStringLiteral("#ff0000"));
        registrar_cor(QStringLiteral("op_modified"), QStringLiteral("Modificado"), QStringLiteral("#ff6200"));
        registrar_cor(QStringLiteral("op_moved"), QStringLiteral("Movido"), QStringLiteral("#ff00ff"));
        registrar_cor(QStringLiteral("op_scanned"), QStringLiteral("Escaneado"), QStringLiteral("#808080"));
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            QStringLiteral("Erro ao atualizar textos traduzidos em BaseGerador"), true);
    }
}

QVariantList BaseGerador::_obter_dados() const {
    QVariantList rows;
    try {
        const QString conn_name = QStringLiteral("stats_conn_%1_%2")
                                      .arg(reinterpret_cast<quintptr>(this))
                                      .arg(QDateTime::currentMSecsSinceEpoch());
        bool ok = true;

        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name);
            db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY"));
            db.setDatabaseName(db_path_);

            if (!db.open()) {
                source::utils::LogManager::get_logger().error(
                    QStringLiteral("Erro SQL ao abrir base: %1").arg(db.lastError().text()), true);
                ok = false;
            } else {
                QSqlQuery pragma(db);
                pragma.exec(QStringLiteral("PRAGMA journal_mode=WAL"));

                QSqlQuery query(db);
                query.prepare(QStringLiteral(
                    "SELECT tipo_operacao, tipo, timestamp, size_mb, size_tb, size_gb, size_kb, size_b, "
                    "dir_atual, dir_anterior, nome, '' AS nome_anterior "
                    "FROM monitoramento WHERE timestamp IS NOT NULL"));
                if (!query.exec()) {
                    source::utils::LogManager::get_logger().error(
                        QStringLiteral("Erro SQL ao consultar monitoramento: %1").arg(query.lastError().text()),
                        true);
                    ok = false;
                } else {
                    while (query.next()) {
                        QVariantMap row;
                        const QSqlRecord rec = query.record();
                        for (int i = 0; i < rec.count(); ++i) {
                            row.insert(rec.fieldName(i), query.value(i));
                        }
                        rows.append(row);
                    }
                }

                db.close();
            }
        }

        QSqlDatabase::removeDatabase(conn_name);
        if (!ok) return {};
        return loc_ ? _traduzir_dados(rows) : rows;
    } catch (...) {
        source::utils::LogManager::get_logger().error(QStringLiteral("Erro ao obter dados em BaseGerador"), true);
        return {};
    }
}

QVariantList BaseGerador::_traduzir_dados(const QVariantList& dados) const {
    QVariantList out;
    const auto mapa_ops = _obter_mapeamento_operacoes();
    const auto mapa_tipos = _obter_mapeamento_tipos();
    for (const QVariant& v : dados) {
        QVariantMap row = v.toMap();
        const QString op = row.value(QStringLiteral("tipo_operacao")).toString();
        const QString tipo = row.value(QStringLiteral("tipo")).toString();
        if (mapa_ops.contains(op)) row.insert(QStringLiteral("tipo_operacao"), mapa_ops.value(op));
        if (mapa_tipos.contains(tipo)) row.insert(QStringLiteral("tipo"), mapa_tipos.value(tipo));
        out.append(row);
    }
    return out;
}

QHash<QString, QString> BaseGerador::_obter_mapeamento_operacoes() const {
    const QString op_added = obter_texto("op_added", QStringLiteral("Adicionado"));
    const QString op_deleted = obter_texto("op_deleted", QStringLiteral("Excluído"));
    const QString op_modified = obter_texto("op_modified", QStringLiteral("Modificado"));
    const QString op_renamed = obter_texto("op_renamed", QStringLiteral("Renomeado"));
    const QString op_moved = obter_texto("op_moved", QStringLiteral("Movido"));
    const QString op_scanned = obter_texto("op_scanned", QStringLiteral("Escaneado"));

    QHash<QString, QString> map;
    auto add = [&map](const QString& k, const QString& v) { map.insert(k, v); };

    add(QStringLiteral("op_added"), op_added);
    add(QStringLiteral("op_deleted"), op_deleted);
    add(QStringLiteral("op_modified"), op_modified);
    add(QStringLiteral("op_renamed"), op_renamed);
    add(QStringLiteral("op_moved"), op_moved);
    add(QStringLiteral("op_scanned"), op_scanned);

    add(op_added, op_added);
    add(op_deleted, op_deleted);
    add(op_modified, op_modified);
    add(op_renamed, op_renamed);
    add(op_moved, op_moved);
    add(op_scanned, op_scanned);

    add(QStringLiteral("Added"), op_added);
    add(QStringLiteral("Deleted"), op_deleted);
    add(QStringLiteral("Modified"), op_modified);
    add(QStringLiteral("Renamed"), op_renamed);
    add(QStringLiteral("Moved"), op_moved);
    add(QStringLiteral("Scanned"), op_scanned);
    add(QStringLiteral("added"), op_added);
    add(QStringLiteral("deleted"), op_deleted);
    add(QStringLiteral("modified"), op_modified);
    add(QStringLiteral("renamed"), op_renamed);
    add(QStringLiteral("moved"), op_moved);
    add(QStringLiteral("scanned"), op_scanned);

    add(QStringLiteral("Adicionado"), op_added);
    add(QStringLiteral("Excluído"), op_deleted);
    add(QStringLiteral("Modificado"), op_modified);
    add(QStringLiteral("Renomeado"), op_renamed);
    add(QStringLiteral("Movido"), op_moved);
    add(QStringLiteral("Escaneado"), op_scanned);

    add(QStringLiteral("Añadido"), op_added);
    add(QStringLiteral("Eliminado"), op_deleted);
    add(QStringLiteral("Renombrado"), op_renamed);

    add(QStringLiteral("Ajouté"), op_added);
    add(QStringLiteral("Supprimé"), op_deleted);
    add(QStringLiteral("Modifié"), op_modified);
    add(QStringLiteral("Renommé"), op_renamed);
    add(QStringLiteral("Déplacé"), op_moved);
    add(QStringLiteral("Numérisé"), op_scanned);

    add(QStringLiteral("Aggiunto"), op_added);
    add(QStringLiteral("Eliminato"), op_deleted);
    add(QStringLiteral("Modificato"), op_modified);
    add(QStringLiteral("Rinominato"), op_renamed);
    add(QStringLiteral("Spostato"), op_moved);
    add(QStringLiteral("Scansionato"), op_scanned);

    add(QStringLiteral("Hinzugefügt"), op_added);
    add(QStringLiteral("Gelöscht"), op_deleted);
    add(QStringLiteral("Geändert"), op_modified);
    add(QStringLiteral("Umbenannt"), op_renamed);
    add(QStringLiteral("Verschoben"), op_moved);
    add(QStringLiteral("Gescannt"), op_scanned);

    return map;
}

QHash<QString, QString> BaseGerador::_obter_mapeamento_tipos() const {
    const QString unknown = obter_texto("unknown", QStringLiteral("Desconhecido"));
    const QString file_unknown = obter_texto("file_unknown", unknown);
    const QString folder = obter_texto("folder", QStringLiteral("Pasta"));
    const QString file_video = obter_texto("file_video", QStringLiteral("Vídeo"));
    const QString file_image = obter_texto("file_image", QStringLiteral("Imagem"));
    const QString file_audio = obter_texto("file_audio", QStringLiteral("Áudio"));
    const QString file_source_code = obter_texto("file_source_code", QStringLiteral("Código fonte"));
    const QString file_document = obter_texto("file_document", QStringLiteral("Documento"));
    const QString file_spreadsheet = obter_texto("file_spreadsheet", QStringLiteral("Planilha"));
    const QString file_presentation = obter_texto("file_presentation", QStringLiteral("Apresentação"));
    const QString file_database = obter_texto("file_database", QStringLiteral("Banco de dados"));
    const QString file_executable = obter_texto("file_executable", QStringLiteral("Executável"));
    const QString file_temp = obter_texto("file_temp", QStringLiteral("Temporário"));
    const QString file_compressed = obter_texto("file_compressed", QStringLiteral("Compactado"));
    const QString archive = obter_texto("archive", QStringLiteral("Arquivo"));
    const QString file_backup = obter_texto("file_backup", QStringLiteral("Backup"));
    const QString file_log = obter_texto("file_log", QStringLiteral("Log"));
    const QString file_config = obter_texto("file_config", QStringLiteral("Configuração"));

    QHash<QString, QString> map;
    auto add = [&map](const QString& k, const QString& v) { map.insert(k, v); };

    // Inglês.
    add(QStringLiteral("unknown"), unknown);
    add(QStringLiteral("Unknown"), unknown);
    add(QStringLiteral("file_unknown"), unknown);
    add(QStringLiteral("unknown file"), unknown);
    add(QStringLiteral("Unknown file"), unknown);
    add(QStringLiteral("Unknown File"), unknown);
    add(file_unknown, unknown);
    add(QStringLiteral("folder"), folder);
    add(QStringLiteral("Folder"), folder);
    add(QStringLiteral("video"), file_video);
    add(QStringLiteral("Video"), file_video);
    add(QStringLiteral("image"), file_image);
    add(QStringLiteral("Image"), file_image);
    add(QStringLiteral("audio"), file_audio);
    add(QStringLiteral("Audio"), file_audio);
    add(QStringLiteral("source code"), file_source_code);
    add(QStringLiteral("Source code"), file_source_code);
    add(QStringLiteral("document"), file_document);
    add(QStringLiteral("Document"), file_document);
    add(QStringLiteral("spreadsheet"), file_spreadsheet);
    add(QStringLiteral("Spreadsheet"), file_spreadsheet);
    add(QStringLiteral("presentation"), file_presentation);
    add(QStringLiteral("Presentation"), file_presentation);
    add(QStringLiteral("database"), file_database);
    add(QStringLiteral("Database"), file_database);
    add(QStringLiteral("executable"), file_executable);
    add(QStringLiteral("Executable"), file_executable);
    add(QStringLiteral("temporary"), file_temp);
    add(QStringLiteral("Temporary"), file_temp);
    add(QStringLiteral("compressed"), file_compressed);
    add(QStringLiteral("Compressed"), file_compressed);
    add(QStringLiteral("archive"), archive);
    add(QStringLiteral("Archive"), archive);
    add(QStringLiteral("backup"), file_backup);
    add(QStringLiteral("Backup"), file_backup);
    add(QStringLiteral("log"), file_log);
    add(QStringLiteral("Log"), file_log);
    add(QStringLiteral("config"), file_config);
    add(QStringLiteral("Config"), file_config);

    // Português.
    add(QStringLiteral("pasta"), folder);
    add(QStringLiteral("Pasta"), folder);
    add(QStringLiteral("vídeo"), file_video);
    add(QStringLiteral("Vídeo"), file_video);
    add(QStringLiteral("imagem"), file_image);
    add(QStringLiteral("Imagem"), file_image);
    add(QStringLiteral("áudio"), file_audio);
    add(QStringLiteral("Áudio"), file_audio);
    add(QStringLiteral("código fonte"), file_source_code);
    add(QStringLiteral("Código fonte"), file_source_code);
    add(QStringLiteral("documento"), file_document);
    add(QStringLiteral("Documento"), file_document);
    add(QStringLiteral("planilha"), file_spreadsheet);
    add(QStringLiteral("Planilha"), file_spreadsheet);
    add(QStringLiteral("apresentação"), file_presentation);
    add(QStringLiteral("Apresentação"), file_presentation);
    add(QStringLiteral("banco de dados"), file_database);
    add(QStringLiteral("Banco de dados"), file_database);
    add(QStringLiteral("executável"), file_executable);
    add(QStringLiteral("Executável"), file_executable);
    add(QStringLiteral("temporário"), file_temp);
    add(QStringLiteral("Temporário"), file_temp);
    add(QStringLiteral("compactado"), file_compressed);
    add(QStringLiteral("Compactado"), file_compressed);
    add(QStringLiteral("arquivo"), archive);
    add(QStringLiteral("Arquivo"), archive);
    add(QStringLiteral("configuração"), file_config);
    add(QStringLiteral("Configuração"), file_config);
    add(QStringLiteral("desconhecido"), unknown);
    add(QStringLiteral("Desconhecido"), unknown);
    add(QStringLiteral("arquivo desconhecido"), unknown);
    add(QStringLiteral("Arquivo Desconhecido"), unknown);

    // Espanhol.
    add(QStringLiteral("carpeta"), folder);
    add(QStringLiteral("Carpeta"), folder);
    add(QStringLiteral("imagen"), file_image);
    add(QStringLiteral("Imagen"), file_image);
    add(QStringLiteral("código fuente"), file_source_code);
    add(QStringLiteral("Código fuente"), file_source_code);
    add(QStringLiteral("hoja de cálculo"), file_spreadsheet);
    add(QStringLiteral("Hoja de cálculo"), file_spreadsheet);
    add(QStringLiteral("presentación"), file_presentation);
    add(QStringLiteral("Presentación"), file_presentation);
    add(QStringLiteral("base de datos"), file_database);
    add(QStringLiteral("Base de datos"), file_database);
    add(QStringLiteral("ejecutable"), file_executable);
    add(QStringLiteral("Ejecutable"), file_executable);
    add(QStringLiteral("temporal"), file_temp);
    add(QStringLiteral("Temporal"), file_temp);
    add(QStringLiteral("comprimido"), file_compressed);
    add(QStringLiteral("Comprimido"), file_compressed);
    add(QStringLiteral("respaldo"), file_backup);
    add(QStringLiteral("Respaldo"), file_backup);
    add(QStringLiteral("copia de seguridad"), file_backup);
    add(QStringLiteral("Copia de seguridad"), file_backup);
    add(QStringLiteral("registro"), file_log);
    add(QStringLiteral("Registro"), file_log);
    add(QStringLiteral("configuración"), file_config);
    add(QStringLiteral("Configuración"), file_config);
    add(QStringLiteral("desconocido"), unknown);
    add(QStringLiteral("Desconocido"), unknown);
    add(QStringLiteral("archivo desconocido"), unknown);
    add(QStringLiteral("Archivo Desconocido"), unknown);

    // Francês.
    add(QStringLiteral("dossier"), folder);
    add(QStringLiteral("Dossier"), folder);
    add(QStringLiteral("vidéo"), file_video);
    add(QStringLiteral("Vidéo"), file_video);
    add(QStringLiteral("code source"), file_source_code);
    add(QStringLiteral("Code source"), file_source_code);
    add(QStringLiteral("feuille de calcul"), file_spreadsheet);
    add(QStringLiteral("Feuille de calcul"), file_spreadsheet);
    add(QStringLiteral("tableur"), file_spreadsheet);
    add(QStringLiteral("Tableur"), file_spreadsheet);
    add(QStringLiteral("présentation"), file_presentation);
    add(QStringLiteral("Présentation"), file_presentation);
    add(QStringLiteral("base de données"), file_database);
    add(QStringLiteral("Base de données"), file_database);
    add(QStringLiteral("exécutable"), file_executable);
    add(QStringLiteral("Exécutable"), file_executable);
    add(QStringLiteral("temporaire"), file_temp);
    add(QStringLiteral("Temporaire"), file_temp);
    add(QStringLiteral("compressé"), file_compressed);
    add(QStringLiteral("Compressé"), file_compressed);
    add(QStringLiteral("sauvegarde"), file_backup);
    add(QStringLiteral("Sauvegarde"), file_backup);
    add(QStringLiteral("journal"), file_log);
    add(QStringLiteral("Journal"), file_log);
    add(QStringLiteral("configuration"), file_config);
    add(QStringLiteral("Configuration"), file_config);
    add(QStringLiteral("inconnu"), unknown);
    add(QStringLiteral("Inconnu"), unknown);
    add(QStringLiteral("fichier inconnu"), unknown);
    add(QStringLiteral("Fichier Inconnu"), unknown);

    // Italiano.
    add(QStringLiteral("cartella"), folder);
    add(QStringLiteral("Cartella"), folder);
    add(QStringLiteral("immagine"), file_image);
    add(QStringLiteral("Immagine"), file_image);
    add(QStringLiteral("codice sorgente"), file_source_code);
    add(QStringLiteral("Codice sorgente"), file_source_code);
    add(QStringLiteral("foglio di calcolo"), file_spreadsheet);
    add(QStringLiteral("Foglio di calcolo"), file_spreadsheet);
    add(QStringLiteral("presentazione"), file_presentation);
    add(QStringLiteral("Presentazione"), file_presentation);
    add(QStringLiteral("eseguibile"), file_executable);
    add(QStringLiteral("Eseguibile"), file_executable);
    add(QStringLiteral("temporaneo"), file_temp);
    add(QStringLiteral("Temporaneo"), file_temp);
    add(QStringLiteral("archivio"), archive);
    add(QStringLiteral("Archivio"), archive);
    add(QStringLiteral("compresso"), file_compressed);
    add(QStringLiteral("Compresso"), file_compressed);
    add(QStringLiteral("configurazione"), file_config);
    add(QStringLiteral("Configurazione"), file_config);
    add(QStringLiteral("sconosciuto"), unknown);
    add(QStringLiteral("Sconosciuto"), unknown);
    add(QStringLiteral("file sconosciuto"), unknown);
    add(QStringLiteral("File Sconosciuto"), unknown);

    // Alemão.
    add(QStringLiteral("ordner"), folder);
    add(QStringLiteral("Ordner"), folder);
    add(QStringLiteral("bild"), file_image);
    add(QStringLiteral("Bild"), file_image);
    add(QStringLiteral("quellcode"), file_source_code);
    add(QStringLiteral("Quellcode"), file_source_code);
    add(QStringLiteral("dokument"), file_document);
    add(QStringLiteral("Dokument"), file_document);
    add(QStringLiteral("tabellenkalkulation"), file_spreadsheet);
    add(QStringLiteral("Tabellenkalkulation"), file_spreadsheet);
    add(QStringLiteral("präsentation"), file_presentation);
    add(QStringLiteral("Präsentation"), file_presentation);
    add(QStringLiteral("datenbank"), file_database);
    add(QStringLiteral("Datenbank"), file_database);
    add(QStringLiteral("ausführbare datei"), file_executable);
    add(QStringLiteral("Ausführbare Datei"), file_executable);
    add(QStringLiteral("temporär"), file_temp);
    add(QStringLiteral("Temporär"), file_temp);
    add(QStringLiteral("archiv"), archive);
    add(QStringLiteral("Archiv"), archive);
    add(QStringLiteral("komprimiert"), file_compressed);
    add(QStringLiteral("Komprimiert"), file_compressed);
    add(QStringLiteral("sicherung"), file_backup);
    add(QStringLiteral("Sicherung"), file_backup);
    add(QStringLiteral("protokoll"), file_log);
    add(QStringLiteral("Protokoll"), file_log);
    add(QStringLiteral("konfiguration"), file_config);
    add(QStringLiteral("Konfiguration"), file_config);
    add(QStringLiteral("unbekannt"), unknown);
    add(QStringLiteral("Unbekannt"), unknown);
    add(QStringLiteral("unbekannte datei"), unknown);
    add(QStringLiteral("Unbekannte Datei"), unknown);

    return map;
}

QVariant BaseGerador::_criar_grafico_sem_dados(const QString& titulo) const {
    QVariantMap out;
    out.insert(QStringLiteral("kind"), QStringLiteral("empty"));
    out.insert(QStringLiteral("title"), titulo);
    out.insert(QStringLiteral("message"), obter_texto("no_data_to_plot", QStringLiteral("Sem dados para exibir")));
    out.insert(QStringLiteral("rows"), QVariantList{});
    return out;
}

QString BaseGerador::obter_texto(const QString& chave, const QString& fallback) const {
    if (!loc_) return fallback;
    QString out = fallback;
    QMetaObject::invokeMethod(loc_, "get_text", invoke_connection_type(loc_),
                              Q_RETURN_ARG(QString, out), Q_ARG(QString, chave), Q_ARG(QString, fallback));
    return out;
}

QVariantMap BaseGerador::criar_resumo_por_coluna(const QVariantList& dados, const QString& coluna, int top_n) const {
    QHash<QString, int> contagem;
    for (const QVariant& v : dados) {
        const QString key = v.toMap().value(coluna).toString().trimmed();
        if (!key.isEmpty()) contagem[key] += 1;
    }

    QList<QPair<QString, int>> itens;
    itens.reserve(contagem.size());
    for (auto it = contagem.constBegin(); it != contagem.constEnd(); ++it) itens.append({it.key(), it.value()});
    std::sort(itens.begin(), itens.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
    if (top_n > 0 && itens.size() > top_n) itens = itens.mid(0, top_n);

    QVariantList labels;
    QVariantList values;
    for (const auto& p : itens) {
        labels.append(p.first);
        values.append(p.second);
    }

    QVariantMap out;
    out.insert(QStringLiteral("labels"), labels);
    out.insert(QStringLiteral("values"), values);
    return out;
}

double BaseGerador::extrair_tamanho_bytes(const QVariantMap& row) const {
    const QList<QPair<QString, double>> campos = {
        {QStringLiteral("size_tb"), 1024.0 * 1024.0 * 1024.0 * 1024.0},
        {QStringLiteral("size_gb"), 1024.0 * 1024.0 * 1024.0},
        {QStringLiteral("size_mb"), 1024.0 * 1024.0},
        {QStringLiteral("size_kb"), 1024.0},
        {QStringLiteral("size_b"), 1.0}
    };

    for (const auto& c : campos) {
        const QVariant v = row.value(c.first);
        bool ok = false;
        const double n = v.toString().replace(',', '.').toDouble(&ok);
        if (ok && n > 0.0) return n * c.second;
    }
    return -1.0;
}

QDateTime BaseGerador::extrair_timestamp(const QVariantMap& row) const {
    const QVariant v = row.value(QStringLiteral("timestamp"));
    if (v.canConvert<QDateTime>()) {
        const QDateTime dt = v.toDateTime();
        if (dt.isValid()) return dt;
    }

    const QString s = v.toString().trimmed();
    if (s.isEmpty()) return {};

    bool ok_epoch = false;
    const qlonglong epoch_raw = s.toLongLong(&ok_epoch);
    if (ok_epoch) {
        const int digits = s.startsWith('-') ? (s.size() - 1) : s.size();
        if (digits >= 16) return QDateTime::fromMSecsSinceEpoch(epoch_raw / 1000);
        if (digits >= 13) return QDateTime::fromMSecsSinceEpoch(epoch_raw);
        return QDateTime::fromSecsSinceEpoch(epoch_raw);
    }

    auto parse_with_formats = [&s](const QStringList& formats) -> QDateTime {
        for (const QString& fmt : formats) {
            const QDateTime dt = QDateTime::fromString(s, fmt);
            if (dt.isValid()) return dt;
        }
        return {};
    };

    QDateTime dt = QDateTime::fromString(s, Qt::ISODateWithMs);
    if (dt.isValid()) return dt;

    dt = QDateTime::fromString(s, Qt::ISODate);
    if (dt.isValid()) return dt;

    const QString s_iso_guess = (s.contains(' ') && (s.contains('T') == false)) ? QString(s).replace(' ', 'T') : s;
    if (s_iso_guess != s) {
        dt = QDateTime::fromString(s_iso_guess, Qt::ISODateWithMs);
        if (dt.isValid()) return dt;
        dt = QDateTime::fromString(s_iso_guess, Qt::ISODate);
        if (dt.isValid()) return dt;
    }

    dt = parse_with_formats({
        QStringLiteral("yyyy-MM-dd HH:mm:ss"),
        QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"),
        QStringLiteral("yyyy/MM/dd HH:mm:ss"),
        QStringLiteral("yyyy/MM/dd HH:mm:ss.zzz"),
        QStringLiteral("dd/MM/yyyy HH:mm:ss"),
        QStringLiteral("dd/MM/yyyy HH:mm:ss.zzz"),
        QStringLiteral("MM/dd/yyyy HH:mm:ss"),
        QStringLiteral("MM/dd/yyyy HH:mm:ss.zzz"),
        QStringLiteral("dd-MM-yyyy HH:mm:ss"),
        QStringLiteral("dd-MM-yyyy HH:mm:ss.zzz"),
        QStringLiteral("yyyy-MM-dd"),
        QStringLiteral("dd/MM/yyyy"),
        QStringLiteral("MM/dd/yyyy"),
    });
    if (dt.isValid()) return dt;

    return {};
}

QString BaseGerador::normalizar_caminho(const QString& dir_val, const QString& nome_val) const {
    QString p = dir_val.trimmed();
    QString n = nome_val.trimmed();
    if (p.isEmpty() && n.isEmpty()) return {};
    if (!p.isEmpty() && !n.isEmpty()) p = QDir::cleanPath(QDir(p).filePath(n));
    else if (!n.isEmpty()) p = QDir::cleanPath(n);
    else p = QDir::cleanPath(p);
    return p;
}

}  // namespace source::data
