#include "gmet_09_ExtrairMetadadosBancoDados.h"

#include <algorithm>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFileInfo>
#include <QSet>

#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

namespace {
const QSet<QString> kDatabaseExts = {
    ".dat", ".db", ".sqlite", ".mdb", ".accdb", ".sav", ".spss", ".db-journal", ".sql", ".dbf", ".ndb", ".frm",
    ".myd", ".myi", ".sqlite3", ".sl3", ".sdb", ".cdb", ".pdb", ".gdb", ".ora", ".dmp", ".par", ".mdf", ".ldf",
    ".idb", ".ibd", ".sqlite2", ".4db", ".4dd", ".adf", ".adp", ".alf", ".ask", ".btr", ".cat", ".daconnections",
    ".daschema", ".db2", ".db3", ".dbc", ".kexi", ".kexic", ".kexis", ".lirs", ".ndf", ".nwdb", ".pan", ".pdm",
    ".pdi", ".pnz", ".rpd", ".rsd", ".sbf", ".sds", ".sqlitedb", ".udl", ".wdb", ".abs", ".abx", ".adb", ".dta"
};

const QSet<QString> kGisExts = {
    ".shp", ".shx", ".dbf", ".prj", ".sbn", ".sbx", ".gpx", ".kml", ".kmz", ".geojson", ".topojson", ".gdb",
    ".mxd", ".aprx", ".lyr", ".lyrx", ".qgs", ".qgz", ".mbtiles", ".geotiff", ".las", ".laz"
};

const QSet<QString> kSqliteLikeExts = {
    ".sqlite", ".db", ".sqlite3", ".sl3", ".sqlite2", ".sqlitedb", ".db3"
};

bool has_ext(const QString& nome_lower, const QSet<QString>& exts) {
    for (const QString& ext : exts) {
        if (!ext.isEmpty() && nome_lower.endsWith(ext)) return true;
    }
    return false;
}

QString detectar_ext(const QString& nome_lower, const QSet<QString>& exts) {
    QString melhor;
    for (const QString& ext : exts) {
        if (!ext.isEmpty() && nome_lower.endsWith(ext) && ext.size() > melhor.size()) {
            melhor = ext;
        }
    }
    return melhor;
}

QString detectar_ext_suportada(const QString& nome_lower) {
    const QString ext_db = detectar_ext(nome_lower, kDatabaseExts);
    const QString ext_gis = detectar_ext(nome_lower, kGisExts);
    return ext_gis.size() > ext_db.size() ? ext_gis : ext_db;
}

bool possui_header_sqlite(const QString& caminho) {
    QFile f(caminho);
    if (!f.open(QIODevice::ReadOnly)) return false;
    const QByteArray head = f.read(16);
    return head.startsWith("SQLite format 3");
}
}  // namespace

QVariantMap extrair_metadados_banco_dados(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap m;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists() || !fi.isFile()) return m;

        const QString nome_lower = fi.fileName().toLower();
        const bool is_database = has_ext(nome_lower, kDatabaseExts);
        const bool is_gis = has_ext(nome_lower, kGisExts);
        if (!is_database && !is_gis) return m;

        const QString ext = detectar_ext_suportada(nome_lower);
        if (!ext.isEmpty()) m.insert("formato_banco", ext);

        const bool sqlite_like = is_database && (has_ext(nome_lower, kSqliteLikeExts) || possui_header_sqlite(caminho));
        if (sqlite_like) {
            const QString conn = QStringLiteral("gmet_db_%1").arg(reinterpret_cast<quintptr>(caminho.constData()));
            {
                auto db = QSqlDatabase::addDatabase("QSQLITE", conn);
                db.setDatabaseName(caminho);
                if (db.open()) {
                    QSqlQuery q(db);
                    q.exec("SELECT name FROM sqlite_master WHERE type='table'");
                    QStringList tabelas;
                    while (q.next()) tabelas << q.value(0).toString();
                    m.insert("tabelas", tabelas.size());
                    m.insert("nomes_tabelas", tabelas.mid(0, 10).join(", "));
                    db.close();
                }
            }
            QSqlDatabase::removeDatabase(conn);
        }

        if (is_gis) {
            m.insert("tipo_geodado", QStringLiteral("GIS"));
            m.insert("camadas", 1);
        }

        if (!m.contains("tabelas") && !is_gis) {
            const int tabelas_estimadas = std::max(1, static_cast<int>(fi.size() / (2.0 * 1024.0 * 1024.0)));
            m.insert("tabelas", tabelas_estimadas);
            m.insert("tabelas_estimadas", tabelas_estimadas);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro geral ao extrair metadados do banco de dados", true);
    }
    return m;
}

}  // namespace source::services::GerenciamentoMetadados
