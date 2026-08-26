#include "mdir_01_multi_diretorio.h"

#include <memory>
#include <unordered_map>
#include <vector>

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QThread>

#include "source/security/secore_01_postgres_utils.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_03_DiretorioScanner.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/CaminhoPersistenteUtils.h"

namespace source::security {
namespace {

struct ProfileState {
    QString id;
    QString name;
    QString primary_directory;
    QStringList directories;
};

struct RuntimeState {
    QString profile_id;
    QString profile_name;
    QString primary_directory;
    QStringList directories;
    std::vector<std::unique_ptr<source::services::Observador>> observers;
};

std::unordered_map<QObject*, std::shared_ptr<RuntimeState>>& all_runtimes() {
    static std::unordered_map<QObject*, std::shared_ptr<RuntimeState>> runtimes;
    return runtimes;
}

QString storage_path() {
    return QDir(source::utils::obter_caminho_persistente())
        .filePath(QStringLiteral("security/multidiretorio_profiles.json"));
}

QString normalize_directory(const QString& directory) {
    QFileInfo info(directory);
    QString path = info.absoluteFilePath();
    if (path.isEmpty()) {
        path = QDir(directory).absolutePath();
    }
    return QDir::cleanPath(path);
}

QString profile_name_for_directory(const QString& directory) {
    QFileInfo info(directory);
    const QString base = info.fileName().trimmed();
    return base.isEmpty() ? directory : base;
}

QString profile_id_for_directory(const QString& directory) {
    const QByteArray digest =
        QCryptographicHash::hash(normalize_directory(directory).toUtf8(), QCryptographicHash::Sha1).toHex();
    return QStringLiteral("profile_%1").arg(QString::fromLatin1(digest.left(12)));
}

QList<ProfileState> load_profiles_from_legacy_json() {
    QList<ProfileState> profiles;
    QFile file(storage_path());
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return profiles;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!doc.isArray()) {
        return profiles;
    }

    for (const QJsonValue& value : doc.array()) {
        if (!value.isObject()) {
            continue;
        }
        const QJsonObject obj = value.toObject();
        ProfileState state;
        state.id = obj.value(QStringLiteral("id")).toString();
        state.name = obj.value(QStringLiteral("name")).toString();
        state.primary_directory = normalize_directory(obj.value(QStringLiteral("primary_directory")).toString());

        const QJsonArray dirs = obj.value(QStringLiteral("directories")).toArray();
        for (const QJsonValue& dir_value : dirs) {
            const QString dir = normalize_directory(dir_value.toString());
            if (!dir.isEmpty() && !state.directories.contains(dir)) {
                state.directories.append(dir);
            }
        }

        if (state.primary_directory.isEmpty() && !state.directories.isEmpty()) {
            state.primary_directory = state.directories.first();
        }
        if (state.id.isEmpty() && !state.primary_directory.isEmpty()) {
            state.id = profile_id_for_directory(state.primary_directory);
        }
        if (state.name.isEmpty() && !state.primary_directory.isEmpty()) {
            state.name = profile_name_for_directory(state.primary_directory);
        }
        if (!state.primary_directory.isEmpty() && !state.directories.contains(state.primary_directory)) {
            state.directories.prepend(state.primary_directory);
        }
        if (!state.id.isEmpty()) {
            profiles.append(state);
        }
    }

    return profiles;
}

void ensure_schema() {
    QSqlDatabase db = source::security::pg::open_database(QStringLiteral("profiles_schema"));
    if (!db.isValid() || !db.isOpen()) {
        return;
    }

    QSqlQuery query(db);
    query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS security_profiles ("
        "profile_id TEXT PRIMARY KEY, "
        "name TEXT, "
        "primary_directory TEXT, "
        "updated_at TEXT)"));
    query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS security_profile_directories ("
        "profile_id TEXT NOT NULL, "
        "directory TEXT NOT NULL, "
        "position INTEGER NOT NULL, "
        "PRIMARY KEY(profile_id, directory))"));
    query.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_security_profile_directories_profile_position "
        "ON security_profile_directories (profile_id, position)"));
}

QList<ProfileState> load_profiles_from_database() {
    QList<ProfileState> profiles;
    ensure_schema();

    QSqlDatabase db = source::security::pg::open_database(QStringLiteral("profiles_read"));
    if (!db.isValid() || !db.isOpen()) {
        return profiles;
    }

    QHash<QString, int> profile_positions;
    QSqlQuery profile_query(db);
    if (!profile_query.exec(QStringLiteral(
            "SELECT profile_id, name, primary_directory "
            "FROM security_profiles ORDER BY updated_at DESC, profile_id ASC"))) {
        return profiles;
    }

    while (profile_query.next()) {
        ProfileState state;
        state.id = profile_query.value(0).toString();
        state.name = profile_query.value(1).toString();
        state.primary_directory = normalize_directory(profile_query.value(2).toString());
        if (!state.id.isEmpty()) {
            profile_positions.insert(state.id, profiles.size());
            profiles.append(state);
        }
    }

    if (profiles.isEmpty()) {
        return profiles;
    }

    QSqlQuery directory_query(db);
    if (!directory_query.exec(QStringLiteral(
            "SELECT profile_id, directory "
            "FROM security_profile_directories ORDER BY profile_id ASC, position ASC"))) {
        return profiles;
    }

    while (directory_query.next()) {
        const QString profile_id = directory_query.value(0).toString();
        const int profile_index = profile_positions.value(profile_id, -1);
        if (profile_index < 0 || profile_index >= profiles.size()) {
            continue;
        }

        const QString directory = normalize_directory(directory_query.value(1).toString());
        if (!directory.isEmpty() && !profiles[profile_index].directories.contains(directory)) {
            profiles[profile_index].directories.append(directory);
        }
    }

    for (ProfileState& state : profiles) {
        if (state.primary_directory.isEmpty() && !state.directories.isEmpty()) {
            state.primary_directory = state.directories.first();
        }
        if (!state.primary_directory.isEmpty() && !state.directories.contains(state.primary_directory)) {
            state.directories.prepend(state.primary_directory);
        }
        if (state.name.isEmpty() && !state.primary_directory.isEmpty()) {
            state.name = profile_name_for_directory(state.primary_directory);
        }
    }

    return profiles;
}

void save_profiles(const QList<ProfileState>& profiles) {
    ensure_schema();

    QSqlDatabase db = source::security::pg::open_database(QStringLiteral("profiles_write"));
    if (!db.isValid() || !db.isOpen()) {
        return;
    }

    if (!db.transaction()) {
        return;
    }

    bool ok = true;
    QSqlQuery query(db);
    ok = ok && query.exec(QStringLiteral("DELETE FROM security_profile_directories"));
    ok = ok && query.exec(QStringLiteral("DELETE FROM security_profiles"));

    if (ok) {
        for (const ProfileState& state : profiles) {
            if (state.id.isEmpty()) {
                continue;
            }

            QSqlQuery profile_insert(db);
            profile_insert.prepare(QStringLiteral(
                "INSERT INTO security_profiles (profile_id, name, primary_directory, updated_at) "
                "VALUES (?, ?, ?, ?)"));
            profile_insert.addBindValue(state.id);
            profile_insert.addBindValue(state.name);
            profile_insert.addBindValue(state.primary_directory);
            profile_insert.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
            ok = ok && profile_insert.exec();

            for (int index = 0; ok && index < state.directories.size(); ++index) {
                QSqlQuery dir_insert(db);
                dir_insert.prepare(QStringLiteral(
                    "INSERT INTO security_profile_directories (profile_id, directory, position) "
                    "VALUES (?, ?, ?)"));
                dir_insert.addBindValue(state.id);
                dir_insert.addBindValue(state.directories.at(index));
                dir_insert.addBindValue(index);
                ok = ok && dir_insert.exec();
            }
        }
    }

    if (ok) {
        db.commit();
    } else {
        db.rollback();
    }
}

QList<ProfileState> load_profiles() {
    QList<ProfileState> profiles = load_profiles_from_database();
    if (!profiles.isEmpty()) {
        return profiles;
    }

    const QList<ProfileState> legacy_profiles = load_profiles_from_legacy_json();
    if (!legacy_profiles.isEmpty()) {
        save_profiles(legacy_profiles);
        return legacy_profiles;
    }
    return profiles;
}

ProfileState ensure_profile_for_directory(const QString& directory) {
    const QString normalized = normalize_directory(directory);
    QList<ProfileState> profiles = load_profiles();

    for (ProfileState& profile : profiles) {
        if (profile.primary_directory == normalized || profile.directories.contains(normalized)) {
            if (!profile.directories.contains(normalized)) {
                profile.directories.prepend(normalized);
            }
            if (profile.primary_directory.isEmpty()) {
                profile.primary_directory = normalized;
            }
            if (profile.name.isEmpty()) {
                profile.name = profile_name_for_directory(profile.primary_directory);
            }
            save_profiles(profiles);
            return profile;
        }
    }

    ProfileState created;
    created.id = profile_id_for_directory(normalized);
    created.name = profile_name_for_directory(normalized);
    created.primary_directory = normalized;
    created.directories = {normalized};
    profiles.append(created);
    save_profiles(profiles);
    return created;
}

RuntimeState* runtime_for(QObject* interface_obj) {
    auto& runtimes = all_runtimes();
    const auto it = runtimes.find(interface_obj);
    if (it == runtimes.end()) {
        return nullptr;
    }
    return it->second.get();
}

void persist_runtime_profile(const RuntimeState& runtime) {
    QList<ProfileState> profiles = load_profiles();
    bool updated = false;
    for (ProfileState& profile : profiles) {
        if (profile.id == runtime.profile_id) {
            profile.name = runtime.profile_name;
            profile.primary_directory = runtime.primary_directory;
            profile.directories = runtime.directories;
            updated = true;
            break;
        }
    }

    if (!updated && !runtime.profile_id.isEmpty()) {
        ProfileState profile;
        profile.id = runtime.profile_id;
        profile.name = runtime.profile_name;
        profile.primary_directory = runtime.primary_directory;
        profile.directories = runtime.directories;
        profiles.append(profile);
    }
    save_profiles(profiles);
}

bool thread_is_running(QThread* thread) {
    return thread != nullptr && thread->isRunning();
}

bool observer_has_runtime_activity(const source::services::Observador* observer) {
    if (observer == nullptr) {
        return false;
    }

    return observer->ativo.load() || thread_is_running(observer->thread_scan.data()) ||
           observer->thread_scan != nullptr || observer->scan_worker != nullptr ||
           observer->scanner != nullptr ||
           (observer->thread != nullptr && observer->thread->joinable()) ||
           (observer->thread_buffer != nullptr && observer->thread_buffer->joinable());
}

QString idioma_da_interface(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return QString();
    }

    QObject* loc_obj = interface_obj->property("loc").value<QObject*>();
    auto* loc = qobject_cast<source::ui::LocalizadorQt*>(loc_obj);
    if (loc == nullptr) {
        return QString();
    }
    return loc->property("idioma_atual").toString();
}

void sincronizar_idioma_observador(source::services::Observador* observer, const QString& language) {
    if (observer == nullptr || observer->loc == nullptr || language.trimmed().isEmpty()) {
        return;
    }
    if (observer->loc->property("idioma_atual").toString() == language) {
        return;
    }
    observer->loc->set_idioma(language);
}

}  // namespace

MultiDiretorioCoordinator& MultiDiretorioCoordinator::instance() {
    static MultiDiretorioCoordinator coordinator;
    return coordinator;
}

void MultiDiretorioCoordinator::ensure_storage_schema() const {
    ensure_schema();
}

void MultiDiretorioCoordinator::ensure_profile_for_interface(QObject* interface_obj,
                                                             const QString& primary_directory) {
    if (interface_obj == nullptr || primary_directory.trimmed().isEmpty()) {
        return;
    }

    const ProfileState profile = ensure_profile_for_directory(primary_directory);
    auto& runtimes = all_runtimes();
    auto& runtime_ptr = runtimes[interface_obj];
    if (!runtime_ptr) {
        runtime_ptr.reset(new RuntimeState());
    }
    RuntimeState& runtime = *runtime_ptr;
    runtime.profile_id = profile.id;
    runtime.profile_name = profile.name;
    runtime.primary_directory = profile.primary_directory;
    runtime.directories = profile.directories;

    interface_obj->setProperty("security_profile_id", profile.id);
    interface_obj->setProperty("security_profile_name", profile.name);
    interface_obj->setProperty("security_multidiretorios", profile.directories);
}

bool MultiDiretorioCoordinator::set_profile_name(QObject* interface_obj, const QString& profile_name) {
    if (interface_obj == nullptr) {
        return false;
    }

    RuntimeState* runtime = runtime_for(interface_obj);
    if (runtime == nullptr || runtime->profile_id.isEmpty()) {
        return false;
    }

    const QString trimmed = profile_name.trimmed();
    if (trimmed.isEmpty() || trimmed == runtime->profile_name) {
        return false;
    }

    runtime->profile_name = trimmed;
    persist_runtime_profile(*runtime);
    interface_obj->setProperty("security_profile_name", runtime->profile_name);
    return true;
}

bool MultiDiretorioCoordinator::add_directory(QObject* interface_obj, const QString& directory) {
    if (interface_obj == nullptr || directory.trimmed().isEmpty()) {
        return false;
    }

    RuntimeState* runtime = runtime_for(interface_obj);
    if (runtime == nullptr) {
        ensure_profile_for_interface(interface_obj, interface_obj->property("diretorio_atual").toString());
        runtime = runtime_for(interface_obj);
    }
    if (runtime == nullptr) {
        return false;
    }

    const QString normalized = normalize_directory(directory);
    if (normalized.isEmpty() || runtime->directories.contains(normalized)) {
        return false;
    }

    runtime->directories.append(normalized);
    persist_runtime_profile(*runtime);
    interface_obj->setProperty("security_multidiretorios", runtime->directories);
    return true;
}

bool MultiDiretorioCoordinator::remove_directory(QObject* interface_obj, const QString& directory) {
    if (interface_obj == nullptr) {
        return false;
    }

    RuntimeState* runtime = runtime_for(interface_obj);
    if (runtime == nullptr) {
        return false;
    }

    const QString normalized = normalize_directory(directory);
    if (normalized.isEmpty() || normalized == runtime->primary_directory) {
        return false;
    }
    if (!runtime->directories.removeOne(normalized)) {
        return false;
    }

    persist_runtime_profile(*runtime);
    interface_obj->setProperty("security_multidiretorios", runtime->directories);
    return true;
}

QStringList MultiDiretorioCoordinator::directories(QObject* interface_obj) const {
    const RuntimeState* runtime = runtime_for(interface_obj);
    if (runtime != nullptr && !runtime->directories.isEmpty()) {
        return runtime->directories;
    }
    if (interface_obj != nullptr) {
        const QString primary = normalize_directory(interface_obj->property("diretorio_atual").toString());
        if (!primary.isEmpty()) {
            return {primary};
        }
    }
    return {};
}

QVariantMap MultiDiretorioCoordinator::profile_summary(QObject* interface_obj) const {
    QVariantMap summary;
    const RuntimeState* runtime = runtime_for(interface_obj);
    if (runtime == nullptr) {
        return summary;
    }

    summary.insert(QStringLiteral("monitor_profile_id"), runtime->profile_id);
    summary.insert(QStringLiteral("monitor_profile_name"), runtime->profile_name);
    summary.insert(QStringLiteral("monitor_diretorios_list"), runtime->directories);
    summary.insert(QStringLiteral("monitor_diretorios_count"), runtime->directories.size());
    summary.insert(QStringLiteral("primary_directory"), runtime->primary_directory);
    return summary;
}

QVariantMap MultiDiretorioCoordinator::runtime_scan_summary(QObject* interface_obj) const {
    QVariantMap summary;
    const RuntimeState* runtime = runtime_for(interface_obj);
    if (runtime == nullptr) {
        return summary;
    }

    int total_arquivos = 0;
    int contador_processados = 0;
    int diretorios_em_scan = 0;
    bool scan_em_execucao = false;
    bool scan_pausado = false;

    for (const auto& observer_ptr : runtime->observers) {
        const auto* observer = observer_ptr.get();
        if (observer == nullptr) {
            continue;
        }

        const bool thread_scan_ativa = thread_is_running(observer->thread_scan.data());
        scan_em_execucao = scan_em_execucao || thread_scan_ativa;
        if (thread_scan_ativa) {
            ++diretorios_em_scan;
        }

        scan_pausado = scan_pausado || observer->pausado.load();

        auto* scanner = observer->scanner.data();
        if (scanner == nullptr) {
            continue;
        }

        const int total_local = qMax(0, scanner->total_arquivos);
        const int processados_local = qMax(0, scanner->contador_processados);
        total_arquivos += total_local;
        contador_processados += (total_local > 0)
                                    ? qBound(0, processados_local, total_local)
                                    : processados_local;
        scan_pausado = scan_pausado || scanner->pausado.load();
    }

    const int progresso =
        total_arquivos > 0
            ? qBound(0, static_cast<int>((static_cast<double>(contador_processados) * 100.0) /
                                         static_cast<double>(total_arquivos)),
                     100)
            : 0;

    summary.insert(QStringLiteral("scan_total"), total_arquivos);
    summary.insert(QStringLiteral("scan_processed"), contador_processados);
    summary.insert(QStringLiteral("scan_progress"), progresso);
    summary.insert(QStringLiteral("scan_running"), scan_em_execucao);
    summary.insert(QStringLiteral("scan_paused"), scan_pausado);
    summary.insert(QStringLiteral("scan_running_directories"), diretorios_em_scan);
    summary.insert(QStringLiteral("scan_configured_directories"), runtime->directories.size());
    return summary;
}

source::services::Observador* MultiDiretorioCoordinator::configure_observers(
    QObject* interface_obj,
    const std::function<void(const QVariantMap&)>& callback) {
    if (interface_obj == nullptr) {
        return nullptr;
    }

    RuntimeState* runtime = runtime_for(interface_obj);
    if (runtime == nullptr) {
        ensure_profile_for_interface(interface_obj, interface_obj->property("diretorio_atual").toString());
        runtime = runtime_for(interface_obj);
    }
    if (runtime == nullptr) {
        return nullptr;
    }

    stop_interface(interface_obj);
    runtime = runtime_for(interface_obj);
    if (runtime == nullptr) {
        return nullptr;
    }

    for (const QString& directory : runtime->directories) {
        auto observer = std::make_unique<source::services::Observador>(directory, callback);
        observer->interface = interface_obj;
        sincronizar_idioma_observador(observer.get(), idioma_da_interface(interface_obj));
        runtime->observers.push_back(std::move(observer));
    }

    source::services::Observador* primary =
        runtime->observers.empty() ? nullptr : runtime->observers.front().get();
    interface_obj->setProperty("_observador_cpp_ptr",
                               QVariant::fromValue<qulonglong>(reinterpret_cast<qulonglong>(primary)));

    if (!interface_obj->property("_observador_cpp_cleanup_connected").toBool()) {
        QObject::connect(interface_obj, &QObject::destroyed, [this](QObject* obj) {
            stop_interface(obj);
            all_runtimes().erase(obj);
        });
        interface_obj->setProperty("_observador_cpp_cleanup_connected", true);
    }

    return primary;
}

void MultiDiretorioCoordinator::stop_interface(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return;
    }

    RuntimeState* runtime = runtime_for(interface_obj);
    if (runtime == nullptr) {
        interface_obj->setProperty("_observador_cpp_ptr", QVariant::fromValue<qulonglong>(0));
        return;
    }

    auto observers = std::move(runtime->observers);
    runtime->observers.clear();
    interface_obj->setProperty("_observador_cpp_ptr", QVariant::fromValue<qulonglong>(0));

    for (auto& observer : observers) {
        if (observer != nullptr) {
            try {
                observer->parar();
            } catch (...) {
            }
        }
    }
}

void MultiDiretorioCoordinator::start_interface(QObject* interface_obj) {
    RuntimeState* runtime = runtime_for(interface_obj);
    if (runtime == nullptr) {
        return;
    }

    for (const auto& observer : runtime->observers) {
        if (observer != nullptr && !observer->ativo.load()) {
            observer->iniciar();
        }
    }
}

void MultiDiretorioCoordinator::stop_scan_interface(QObject* interface_obj) {
    RuntimeState* runtime = runtime_for(interface_obj);
    if (runtime == nullptr) {
        return;
    }

    for (const auto& observer : runtime->observers) {
        if (observer != nullptr && thread_is_running(observer->thread_scan.data())) {
            observer->parar_scan();
        }
    }
}

void MultiDiretorioCoordinator::sync_interface_language(QObject* interface_obj,
                                                        const QString& language) const {
    RuntimeState* runtime = runtime_for(interface_obj);
    if (runtime == nullptr) {
        return;
    }

    for (const auto& observer : runtime->observers) {
        sincronizar_idioma_observador(observer.get(), language);
    }
}

bool MultiDiretorioCoordinator::any_scan_running(QObject* interface_obj) const {
    const RuntimeState* runtime = runtime_for(interface_obj);
    if (runtime == nullptr) {
        return false;
    }

    for (const auto& observer : runtime->observers) {
        if (observer != nullptr && thread_is_running(observer->thread_scan.data())) {
            return true;
        }
    }
    return false;
}

bool MultiDiretorioCoordinator::any_active(QObject* interface_obj) const {
    const RuntimeState* runtime = runtime_for(interface_obj);
    if (runtime == nullptr) {
        return false;
    }

    for (const auto& observer : runtime->observers) {
        if (observer != nullptr && observer->ativo.load()) {
            return true;
        }
    }
    return false;
}

source::services::Observador* MultiDiretorioCoordinator::primary_observer(QObject* interface_obj) const {
    const RuntimeState* runtime = runtime_for(interface_obj);
    if (runtime == nullptr || runtime->observers.empty()) {
        return nullptr;
    }
    return runtime->observers.front().get();
}

}  // namespace source::security
