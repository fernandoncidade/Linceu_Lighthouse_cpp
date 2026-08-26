#include "AssociatedApps.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QSet>
#include <QStringList>
#include <QUrl>

#include <string>

#ifdef Q_OS_WIN
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#  include <shlwapi.h>
#  include <shellapi.h>
#endif

namespace source::utils {
namespace {

QString trKey(const char* key, const QString& fallback) {
    const QString source = QString::fromLatin1(key);
    const QString translated = QCoreApplication::translate("LinceuLighthouse", key);
    if (translated.isEmpty() || translated == source) {
        return fallback;
    }
    return translated;
}

QString normalizedExtensionForFile(const QString& filePath) {
    const QString suffix = QFileInfo(filePath).suffix().trimmed().toLower();
    return suffix.isEmpty() ? QString() : QStringLiteral(".") + suffix;
}

void addUnique(QVector<AssociatedApp>& apps, QSet<QString>& seen, const AssociatedApp& app) {
    QString key;
    if (app.kind == AssociatedApp::Kind::SystemDefault) {
        key = QStringLiteral("__system_default__");
    } else {
        key = QFileInfo(app.executablePath).canonicalFilePath().toLower();
        if (key.isEmpty()) {
            key = app.executablePath.toLower();
        }
    }

    if (key.isEmpty() || seen.contains(key)) {
        return;
    }

    seen.insert(key);
    apps.push_back(app);
}

#ifdef Q_OS_WIN
std::wstring toStdWString(const QString& s) {
    return std::wstring(reinterpret_cast<const wchar_t*>(s.utf16()), static_cast<size_t>(s.size()));
}

QString fromWide(const wchar_t* value) {
    return value != nullptr ? QString::fromWCharArray(value) : QString();
}

QString expandEnvironmentStringsIfNeeded(const QString& text) {
    if (!text.contains(QLatin1Char('%'))) {
        return text;
    }

    const std::wstring in = toStdWString(text);
    const DWORD required = ExpandEnvironmentStringsW(in.c_str(), nullptr, 0);
    if (required == 0) {
        return text;
    }

    std::wstring out(required, L'\0');
    const DWORD written = ExpandEnvironmentStringsW(in.c_str(), out.data(), required);
    if (written == 0 || written > required) {
        return text;
    }

    return fromWide(out.c_str());
}

QString readRegistryString(HKEY root, const QString& subKey, const QString& valueName = QString()) {
    const std::wstring key = toStdWString(subKey);
    const std::wstring value = toStdWString(valueName);

    DWORD type = 0;
    DWORD bytes = 0;
    LONG rc = RegGetValueW(root,
                           key.c_str(),
                           valueName.isEmpty() ? nullptr : value.c_str(),
                           RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ,
                           &type,
                           nullptr,
                           &bytes);
    if (rc != ERROR_SUCCESS || bytes == 0) {
        return {};
    }

    std::wstring buffer(bytes / sizeof(wchar_t), L'\0');
    rc = RegGetValueW(root,
                      key.c_str(),
                      valueName.isEmpty() ? nullptr : value.c_str(),
                      RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ,
                      &type,
                      buffer.data(),
                      &bytes);
    if (rc != ERROR_SUCCESS) {
        return {};
    }

    return expandEnvironmentStringsIfNeeded(fromWide(buffer.c_str()).trimmed());
}

QStringList enumRegistryValueNames(HKEY root, const QString& subKey) {
    QStringList names;
    HKEY keyHandle = nullptr;
    const std::wstring key = toStdWString(subKey);

    if (RegOpenKeyExW(root, key.c_str(), 0, KEY_READ, &keyHandle) != ERROR_SUCCESS) {
        return names;
    }

    DWORD valueCount = 0;
    DWORD maxValueNameLen = 0;
    if (RegQueryInfoKeyW(keyHandle,
                         nullptr,
                         nullptr,
                         nullptr,
                         nullptr,
                         nullptr,
                         nullptr,
                         &valueCount,
                         &maxValueNameLen,
                         nullptr,
                         nullptr,
                         nullptr) == ERROR_SUCCESS) {
        for (DWORD i = 0; i < valueCount; ++i) {
            std::wstring name(maxValueNameLen + 2, L'\0');
            DWORD nameLen = static_cast<DWORD>(name.size());
            DWORD type = 0;

            const LONG rc = RegEnumValueW(keyHandle, i, name.data(), &nameLen, nullptr, &type, nullptr, nullptr);
            if (rc == ERROR_SUCCESS && nameLen > 0) {
                const QString n = QString::fromWCharArray(name.data(), static_cast<int>(nameLen)).trimmed();
                if (!n.isEmpty() && n.compare(QStringLiteral("MRUList"), Qt::CaseInsensitive) != 0) {
                    names << n;
                }
            }
        }
    }

    RegCloseKey(keyHandle);
    names.removeDuplicates();
    return names;
}

QStringList enumRegistryStringValues(HKEY root, const QString& subKey) {
    QStringList values;
    HKEY keyHandle = nullptr;
    const std::wstring key = toStdWString(subKey);

    if (RegOpenKeyExW(root, key.c_str(), 0, KEY_READ, &keyHandle) != ERROR_SUCCESS) {
        return values;
    }

    DWORD valueCount = 0;
    DWORD maxValueNameLen = 0;
    DWORD maxValueLen = 0;
    if (RegQueryInfoKeyW(keyHandle,
                         nullptr,
                         nullptr,
                         nullptr,
                         nullptr,
                         nullptr,
                         nullptr,
                         &valueCount,
                         &maxValueNameLen,
                         &maxValueLen,
                         nullptr,
                         nullptr) == ERROR_SUCCESS) {
        for (DWORD i = 0; i < valueCount; ++i) {
            std::wstring name(maxValueNameLen + 2, L'\0');
            DWORD nameLen = static_cast<DWORD>(name.size());
            DWORD type = 0;
            std::wstring data((maxValueLen / sizeof(wchar_t)) + 2, L'\0');
            DWORD dataBytes = static_cast<DWORD>(data.size() * sizeof(wchar_t));

            const LONG rc = RegEnumValueW(keyHandle,
                                          i,
                                          name.data(),
                                          &nameLen,
                                          nullptr,
                                          &type,
                                          reinterpret_cast<LPBYTE>(data.data()),
                                          &dataBytes);
            if (rc == ERROR_SUCCESS && (type == REG_SZ || type == REG_EXPAND_SZ)) {
                const QString n = QString::fromWCharArray(name.data(), static_cast<int>(nameLen));
                if (n.compare(QStringLiteral("MRUList"), Qt::CaseInsensitive) == 0) {
                    continue;
                }
                const QString value = fromWide(data.c_str()).trimmed();
                if (!value.isEmpty()) {
                    values << expandEnvironmentStringsIfNeeded(value);
                }
            }
        }
    }

    RegCloseKey(keyHandle);
    values.removeDuplicates();
    return values;
}

QString executableFromCommand(const QString& command) {
    const QString c = expandEnvironmentStringsIfNeeded(command).trimmed();
    if (c.isEmpty()) {
        return {};
    }

    if (c.startsWith(QLatin1Char('"'))) {
        const int endQuote = c.indexOf(QLatin1Char('"'), 1);
        if (endQuote > 1) {
            return c.mid(1, endQuote - 1).trimmed();
        }
    }

    const int exeIndex = c.toLower().indexOf(QStringLiteral(".exe"));
    if (exeIndex >= 0) {
        return c.left(exeIndex + 4).trimmed();
    }

    const int firstSpace = c.indexOf(QLatin1Char(' '));
    if (firstSpace > 0) {
        return c.left(firstSpace).trimmed();
    }

    return c;
}

QString defaultExecutableForExtension(const QString& extension) {
    if (extension.isEmpty()) {
        return {};
    }

    const std::wstring ext = toStdWString(extension);
    DWORD required = 0;
    HRESULT hr = AssocQueryStringW(ASSOCF_NONE,
                                   ASSOCSTR_EXECUTABLE,
                                   ext.c_str(),
                                   L"open",
                                   nullptr,
                                   &required);
    if (hr != S_FALSE || required == 0) {
        return {};
    }

    std::wstring buffer(required, L'\0');
    hr = AssocQueryStringW(ASSOCF_NONE,
                           ASSOCSTR_EXECUTABLE,
                           ext.c_str(),
                           L"open",
                           buffer.data(),
                           &required);
    if (FAILED(hr)) {
        return {};
    }

    return fromWide(buffer.c_str()).trimmed();
}

QString commandFromProgId(const QString& progId) {
    if (progId.isEmpty()) {
        return {};
    }

    const QString suffix = progId + QStringLiteral("\\shell\\open\\command");
    QString command = readRegistryString(HKEY_CURRENT_USER, QStringLiteral("Software\\Classes\\") + suffix);
    if (!command.isEmpty()) {
        return command;
    }

    command = readRegistryString(HKEY_LOCAL_MACHINE, QStringLiteral("Software\\Classes\\") + suffix);
    if (!command.isEmpty()) {
        return command;
    }

    return readRegistryString(HKEY_CLASSES_ROOT, suffix);
}

QString friendlyNameFromProgId(const QString& progId) {
    QString name = readRegistryString(HKEY_CURRENT_USER, QStringLiteral("Software\\Classes\\") + progId);
    if (!name.isEmpty()) {
        return name;
    }

    name = readRegistryString(HKEY_LOCAL_MACHINE, QStringLiteral("Software\\Classes\\") + progId);
    if (!name.isEmpty()) {
        return name;
    }

    name = readRegistryString(HKEY_CLASSES_ROOT, progId);
    return name.isEmpty() ? progId : name;
}

QString commandFromApplicationExeName(const QString& exeName) {
    if (exeName.isEmpty()) {
        return {};
    }

    const QString suffix = QStringLiteral("Applications\\") + exeName + QStringLiteral("\\shell\\open\\command");
    QString command = readRegistryString(HKEY_CURRENT_USER, QStringLiteral("Software\\Classes\\") + suffix);
    if (!command.isEmpty()) {
        return command;
    }

    command = readRegistryString(HKEY_LOCAL_MACHINE, QStringLiteral("Software\\Classes\\") + suffix);
    if (!command.isEmpty()) {
        return command;
    }

    return readRegistryString(HKEY_CLASSES_ROOT, suffix);
}

void addExecutableFromCommand(QVector<AssociatedApp>& apps,
                              QSet<QString>& seen,
                              const QString& name,
                              const QString& command,
                              const QString& source) {
    const QString exe = executableFromCommand(command);
    if (exe.isEmpty()) {
        return;
    }

    const QFileInfo exeInfo(exe);
    AssociatedApp app;
    app.kind = AssociatedApp::Kind::Executable;
    app.executablePath = exe;
    app.displayName = !name.trimmed().isEmpty()
                          ? name.trimmed()
                          : (!exeInfo.completeBaseName().isEmpty() ? exeInfo.completeBaseName() : exe);
    app.source = source;

    addUnique(apps, seen, app);
}
#endif

}  // namespace

QVector<AssociatedApp> AssociatedApps::appsForFile(const QString& filePath) {
    QVector<AssociatedApp> apps;
    QSet<QString> seen;

    AssociatedApp systemDefault;
    systemDefault.kind = AssociatedApp::Kind::SystemDefault;
    systemDefault.displayName = trKey("associated_app_system_default",
                                      QStringLiteral("Aplicativo padrão do sistema"));
    systemDefault.source = trKey("associated_app_system_default_source",
                                 QStringLiteral("Associação padrão do sistema operacional"));
    addUnique(apps, seen, systemDefault);

#ifdef Q_OS_WIN
    const QString extension = normalizedExtensionForFile(filePath);
    if (extension.isEmpty()) {
        return apps;
    }

    const QString defaultExe = defaultExecutableForExtension(extension);
    if (!defaultExe.isEmpty()) {
        const QFileInfo info(defaultExe);
        AssociatedApp app;
        app.kind = AssociatedApp::Kind::Executable;
        app.displayName = info.completeBaseName().isEmpty() ? defaultExe : info.completeBaseName();
        app.executablePath = defaultExe;
        app.source = trKey("associated_app_windows_default_source",
                           QStringLiteral("Aplicativo padrão registrado no Windows"));
        addUnique(apps, seen, app);
    }

    QStringList progIds;
    progIds << readRegistryString(HKEY_CURRENT_USER, QStringLiteral("Software\\Classes\\") + extension);
    progIds << readRegistryString(HKEY_LOCAL_MACHINE, QStringLiteral("Software\\Classes\\") + extension);
    progIds << readRegistryString(HKEY_CLASSES_ROOT, extension);
    progIds << enumRegistryValueNames(
        HKEY_CURRENT_USER,
        QStringLiteral("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\") +
            extension + QStringLiteral("\\OpenWithProgids"));
    progIds << enumRegistryValueNames(
        HKEY_CURRENT_USER,
        QStringLiteral("Software\\Classes\\") + extension + QStringLiteral("\\OpenWithProgids"));
    progIds << enumRegistryValueNames(
        HKEY_LOCAL_MACHINE,
        QStringLiteral("Software\\Classes\\") + extension + QStringLiteral("\\OpenWithProgids"));
    progIds << enumRegistryValueNames(HKEY_CLASSES_ROOT, extension + QStringLiteral("\\OpenWithProgids"));
    progIds.removeAll(QString());
    progIds.removeDuplicates();

    for (const QString& progId : progIds) {
        const QString command = commandFromProgId(progId);
        addExecutableFromCommand(apps,
                                 seen,
                                 friendlyNameFromProgId(progId),
                                 command,
                                 trKey("associated_app_registered_progid_source",
                                       QStringLiteral("ProgID registrado: %1")).arg(progId));
    }

    QStringList appExeNames;
    appExeNames << enumRegistryStringValues(
        HKEY_CURRENT_USER,
        QStringLiteral("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\") +
            extension + QStringLiteral("\\OpenWithList"));
    appExeNames << enumRegistryValueNames(HKEY_CLASSES_ROOT, extension + QStringLiteral("\\OpenWithList"));
    appExeNames.removeDuplicates();

    for (const QString& exeName : appExeNames) {
        const QString command = commandFromApplicationExeName(exeName);
        addExecutableFromCommand(apps,
                                 seen,
                                 QFileInfo(exeName).completeBaseName(),
                                 command,
                                 trKey("associated_app_open_with_list_source",
                                       QStringLiteral("Lista Abrir com do Windows")));
    }
#else
    Q_UNUSED(filePath)
#endif

    return apps;
}

bool AssociatedApps::openWithSystemDefault(const QString& filePath, QString* errorMessage) {
    const bool ok = QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    if (!ok && errorMessage != nullptr) {
        *errorMessage = trKey("associated_app_error_system_default",
                              QStringLiteral("Não foi possível abrir o arquivo com o aplicativo padrão do sistema."));
    }
    return ok;
}

bool AssociatedApps::openWith(const AssociatedApp& app, const QString& filePath, QString* errorMessage) {
    if (app.kind == AssociatedApp::Kind::SystemDefault) {
        return openWithSystemDefault(filePath, errorMessage);
    }

    if (app.executablePath.trimmed().isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = trKey("associated_app_error_no_executable",
                                  QStringLiteral("Nenhum executável foi definido para abrir o arquivo."));
        }
        return false;
    }

#ifdef Q_OS_WIN
    const std::wstring exe = toStdWString(app.executablePath);
    const std::wstring params = toStdWString(QStringLiteral("\"%1\"").arg(QDir::toNativeSeparators(filePath)));

    HINSTANCE result = ShellExecuteW(nullptr, L"open", exe.c_str(), params.c_str(), nullptr, SW_SHOWNORMAL);
    if (reinterpret_cast<INT_PTR>(result) <= 32) {
        if (errorMessage != nullptr) {
            *errorMessage = trKey("associated_app_error_windows_refused",
                                  QStringLiteral("O Windows recusou a abertura do arquivo com o executável selecionado."));
        }
        return false;
    }
    return true;
#else
    const bool ok = QProcess::startDetached(app.executablePath, QStringList{filePath});
    if (!ok && errorMessage != nullptr) {
        *errorMessage = trKey("associated_app_error_start_failed",
                              QStringLiteral("Não foi possível iniciar o aplicativo selecionado."));
    }
    return ok;
#endif
}

}  // namespace source::utils
