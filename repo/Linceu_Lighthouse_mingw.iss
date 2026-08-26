#define MyAppName "Linceu Lighthouse"
#define MyAppVersion "2026.8.25.0"
#define MyAppPublisher "Fernando Nillsson Cidade"
#define MyAppURL "https://github.com/fernandoncidade"
#define MyAppExeName "Linceu_Lighthouse.exe"
#define DeploySourceDir "D:\MISCELANEAS\VSCode\LinceuLighthouse\Linceu_Lighthouse_mingw"
#define InstallerOutputDir "D:\MISCELANEAS\VSCode\LinceuLighthouse\InnoSetupOutput"

[Setup]
AppId={{71AC66FA-3E2D-4CD6-9C4F-CD1F187FA8B7}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
UninstallDisplayName={#MyAppName}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
UninstallDisplayIcon={app}\{#MyAppExeName}
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
DisableProgramGroupPage=yes
InfoBeforeFile={#DeploySourceDir}\assets\ABOUT\ABOUT_en_US.txt
LicenseFile={#DeploySourceDir}\assets\EULA\EULA_en_US - Linceu Lighthouse.txt
OutputDir={#InstallerOutputDir}
OutputBaseFilename=Linceu_Lighthouse_mingw_v2026.8.25.0
SetupIconFile={#DeploySourceDir}\assets\images\icons\linceu_sem_fundo.ico
SolidCompression=yes
WizardStyle=modern
ShowLanguageDialog=yes
AllowNoIcons=yes
DisableReadyPage=yes
DisableFinishedPage=no

[Languages]
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"; InfoBeforeFile: "{#DeploySourceDir}\assets\ABOUT\ABOUT_pt_BR.txt"; LicenseFile: "{#DeploySourceDir}\assets\EULA\EULA_pt_BR - Linceu Lighthouse.txt"
Name: "english"; MessagesFile: "compiler:Default.isl"; InfoBeforeFile: "{#DeploySourceDir}\assets\ABOUT\ABOUT_en_US.txt"; LicenseFile: "{#DeploySourceDir}\assets\EULA\EULA_en_US - Linceu Lighthouse.txt"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"; InfoBeforeFile: "{#DeploySourceDir}\assets\ABOUT\ABOUT_es_ES.txt"; LicenseFile: "{#DeploySourceDir}\assets\EULA\EULA_es_ES - Linceu Lighthouse.txt"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"; InfoBeforeFile: "{#DeploySourceDir}\assets\ABOUT\ABOUT_fr_FR.txt"; LicenseFile: "{#DeploySourceDir}\assets\EULA\EULA_fr_FR - Linceu Lighthouse.txt"
Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"; InfoBeforeFile: "{#DeploySourceDir}\assets\ABOUT\ABOUT_it_IT.txt"; LicenseFile: "{#DeploySourceDir}\assets\EULA\EULA_it_IT - Linceu Lighthouse.txt"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"; InfoBeforeFile: "{#DeploySourceDir}\assets\ABOUT\ABOUT_de_DE.txt"; LicenseFile: "{#DeploySourceDir}\assets\EULA\EULA_de_DE - Linceu Lighthouse.txt"

[CustomMessages]
brazilianportuguese.AppLanguage=Idioma do aplicativo
brazilianportuguese.SelectAppLang=Selecione o idioma padrão do aplicativo
brazilianportuguese.Portuguese=Português
brazilianportuguese.English=Inglês
brazilianportuguese.Spanish=Espanhol
brazilianportuguese.French=Francês
brazilianportuguese.Italian=Italiano
brazilianportuguese.German=Alemão

english.AppLanguage=Application language
english.SelectAppLang=Select the default application language
english.Portuguese=Portuguese
english.English=English
english.Spanish=Spanish
english.French=French
english.Italian=Italian
english.German=German

spanish.AppLanguage=Idioma de la aplicación
spanish.SelectAppLang=Seleccione el idioma predeterminado de la aplicación
spanish.Portuguese=Portugués
spanish.English=Inglés
spanish.Spanish=Español
spanish.French=Francés
spanish.Italian=Italiano
spanish.German=Alemán

french.AppLanguage=Langue de l'application
french.SelectAppLang=Sélectionnez la langue par défaut de l'application
french.Portuguese=Portugais
french.English=Anglais
french.Spanish=Espagnol
french.French=Français
french.Italian=Italien
french.German=Allemand

italian.AppLanguage=Lingua dell'applicazione
italian.SelectAppLang=Seleziona la lingua predefinita dell'applicazione
italian.Portuguese=Portoghese
italian.English=Inglese
italian.Spanish=Spagnolo
italian.French=Francese
italian.Italian=Italiano
italian.German=Tedesco

german.AppLanguage=Anwendungssprache
german.SelectAppLang=Wählen Sie die Standardsprache der Anwendung
german.Portuguese=Portugiesisch
german.English=Englisch
german.Spanish=Spanisch
german.French=Französisch
german.Italian=Italienisch
german.German=Deutsch

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "langpt_BR"; Description: "{cm:Portuguese}"; GroupDescription: "{cm:AppLanguage}"; Flags: exclusive
Name: "langen_US"; Description: "{cm:English}"; GroupDescription: "{cm:AppLanguage}"; Flags: exclusive unchecked
Name: "langes_ES"; Description: "{cm:Spanish}"; GroupDescription: "{cm:AppLanguage}"; Flags: exclusive unchecked
Name: "langfr_FR"; Description: "{cm:French}"; GroupDescription: "{cm:AppLanguage}"; Flags: exclusive unchecked
Name: "langit_IT"; Description: "{cm:Italian}"; GroupDescription: "{cm:AppLanguage}"; Flags: exclusive unchecked
Name: "langde_DE"; Description: "{cm:German}"; GroupDescription: "{cm:AppLanguage}"; Flags: exclusive unchecked

[Files]
Source: "{#DeploySourceDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; WorkingDir: "{app}"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent; WorkingDir: "{app}"

[ReturnCodes]
6000=UserCancelled
6001=AppAlreadyExists
6002=AnotherInstallationRunning
6003=DiskSpaceFull
6004=RebootRequired
6005=NetworkFailure_DownloadError
6006=NetworkFailure_ConnectionLost
6007=PackageRejectedByPolicy
0=Success

[Code]
procedure CreateLanguageConfigJSON();
var
  FileName: string;
  LanguageCode: string;
  JSONContent: string;
  ConfigDir: string;
begin
  ConfigDir := ExpandConstant('{localappdata}\LinceuLighthouse');
  FileName := ConfigDir + '\language_config.json';

  if not DirExists(ConfigDir) then
    ForceDirectories(ConfigDir);

  if (WizardSilent) then
    LanguageCode := 'en_US'
  else if WizardIsTaskSelected('langpt_BR') then
    LanguageCode := 'pt_BR'
  else if WizardIsTaskSelected('langen_US') then
    LanguageCode := 'en_US'
  else if WizardIsTaskSelected('langes_ES') then
    LanguageCode := 'es_ES'
  else if WizardIsTaskSelected('langfr_FR') then
    LanguageCode := 'fr_FR'
  else if WizardIsTaskSelected('langit_IT') then
    LanguageCode := 'it_IT'
  else if WizardIsTaskSelected('langde_DE') then
    LanguageCode := 'de_DE'
  else
    LanguageCode := 'pt_BR';

  JSONContent := '{' + #13#10;
  JSONContent := JSONContent + '  "idioma": "' + LanguageCode + '"' + #13#10;
  JSONContent := JSONContent + '}';

  SaveStringToFile(FileName, JSONContent, False);
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    CreateLanguageConfigJSON();
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  LocalAppDataPath: string;
begin
  if CurUninstallStep = usUninstall then
  begin
    LocalAppDataPath := ExpandConstant('{localappdata}\LinceuLighthouse');
    if DirExists(LocalAppDataPath) then
    begin
      Log('Removendo o diretório de dados do aplicativo: ' + LocalAppDataPath);
      DelTree(LocalAppDataPath, True, True, True);
    end;
  end;
end;

