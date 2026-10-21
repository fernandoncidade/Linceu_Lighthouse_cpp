[CmdletBinding()]
param(
    [string]$TargetBaseDir = "D:\MISCELANEAS\VSCode\LinceuLighthouse",
    [string]$DistributionName = "all"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Normalize-Path {
    param([string]$Path)
    return [System.IO.Path]::GetFullPath($Path).TrimEnd(
        [System.IO.Path]::DirectorySeparatorChar,
        [System.IO.Path]::AltDirectorySeparatorChar
    )
}

$scriptDir = if ($PSScriptRoot) { $PSScriptRoot } else { Split-Path -Parent $MyInvocation.MyCommand.Path }
$projectRoot = Normalize-Path (Split-Path -Parent $scriptDir)

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host " GERADOR DE DISTRIBUIÇÕES LIMPAS - LINCEU LIGHTHOUSE " -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "Origem do Projeto : $projectRoot"
Write-Host "Destino Base      : $TargetBaseDir"
if ($DistributionName -and $DistributionName -ne "all") {
    Write-Host "Filtro de Distribuição: $DistributionName" -ForegroundColor Magenta
}
Write-Host ""

if (-not (Test-Path $TargetBaseDir)) {
    New-Item -ItemType Directory -Path $TargetBaseDir -Force | Out-Null
    Write-Host "[CRIADO] Diretório base de destino: $TargetBaseDir" -ForegroundColor Green
}

$distributions = @(
    @{
        Name = "Linceu_Lighthouse_msvc"
        BuildDir = (Join-Path $projectRoot "build\build_qt6_msvc\Release")
        TargetDir = (Join-Path $TargetBaseDir "Linceu_Lighthouse_msvc")
        Executable = "Linceu_Lighthouse.exe"
        AllExecutables = @("Linceu_Lighthouse.exe", "linceu-lighthouse-cli.exe")
        IsMock = $false
    },
    @{
        Name = "Linceu_Lighthouse_mingw"
        BuildDir = (Join-Path $projectRoot "build\build_qt6_mingw")
        TargetDir = (Join-Path $TargetBaseDir "Linceu_Lighthouse_mingw")
        Executable = "Linceu_Lighthouse.exe"
        AllExecutables = @("Linceu_Lighthouse.exe", "linceu-lighthouse-cli.exe")
        IsMock = $false
    }
)

if ($DistributionName -and $DistributionName -ne "all") {
    $matched = @()
    foreach ($d in $distributions) {
        if ($d.Name -ieq $DistributionName) {
            $matched += $d
        } elseif ($d.Name -ilike "*$DistributionName*") {
            $matched += $d
        }
    }
    $distributions = $matched
    if ($distributions.Count -eq 0) {
        Write-Warning "Nenhuma distribuição encontrada correspondente a '$DistributionName'."
        return
    }
}

$results = @()

foreach ($dist in $distributions) {
    $distName = $dist.Name
    $destDir = $dist.TargetDir
    $buildDir = $dist.BuildDir

    Write-Host "------------------------------------------------------------" -ForegroundColor Yellow
    Write-Host "Processando e higienizando distribuição: $distName" -ForegroundColor Yellow
    Write-Host "  Diretório : $destDir"

    if (-not (Test-Path $destDir)) {
        Write-Warning "Diretório de distribuição não encontrado: $destDir. Execute o deploy primeiro."
        continue
    }

    # 1. Garantir que os executáveis compilados estejam na pasta de distribuição
    foreach ($exeName in $dist.AllExecutables) {
        $destExe = Join-Path $destDir $exeName
        if (-not (Test-Path $destExe)) {
            $srcExe = Join-Path $buildDir $exeName
            if (Test-Path $srcExe) {
                Write-Host "  Copiando $exeName do build para distribuição..." -ForegroundColor Cyan
                Copy-Item -Path $srcExe -Destination $destExe -Force
            }
        }
    }

    # 2. Remover DLLs não necessárias ou pesadas (ex: compiladores DirectX de shaders em runtime)
    $blacklistedDlls = @("dxcompiler.dll", "dxil.dll")
    foreach ($bDll in $blacklistedDlls) {
        $badDllPath = Join-Path $destDir $bDll
        if (Test-Path $badDllPath) {
            Write-Host "  Removendo DLL desnecessária: $bDll" -ForegroundColor DarkYellow
            Remove-Item -Path $badDllPath -Force
        }
    }

    # 3. Remover resíduos de compilação ou arquivos temporários na raiz da distribuição
    $unwantedPatterns = @("*.ilk", "*.pdb", "*.exp", "*.lib", "*.a", "*.obj", "*.o", "*.ninja*", "*.cmake", "*.tlog", "*.lastbuildstate", "vc_redist.x64.exe", "*.json")
    foreach ($pat in $unwantedPatterns) {
        Get-ChildItem -Path $destDir -Filter $pat -File | ForEach-Object {
            Write-Host "  Removendo resíduo: $($_.Name)" -ForegroundColor DarkGray
            Remove-Item -Path $_.FullName -Force
        }
    }

    # 4. Higienizar pastas de plugins do Qt (remover arquivos que não sejam .dll)
    $pluginFolders = @("generic", "iconengines", "imageformats", "networkinformation", "platforminputcontexts", "platforms", "qmltooling", "sqldrivers", "styles", "tls", "vectorimageformats")
    foreach ($pf in $pluginFolders) {
        $pDir = Join-Path $destDir $pf
        if (Test-Path $pDir) {
            Get-ChildItem -Path $pDir -Recurse -File | Where-Object { $_.Extension -ine ".dll" } | ForEach-Object {
                Remove-Item -Path $_.FullName -Force
            }
        }
    }

    # 5. Higienizar pasta assets (eliminar subpastas recursivas acidentais assets\assets e arquivos não relacionados)
    $assetsDir = Join-Path $destDir "assets"
    if (Test-Path $assetsDir) {
        $subAssets = Join-Path $assetsDir "assets"
        if (Test-Path $subAssets) {
            Remove-Item -Path $subAssets -Recurse -Force
        }
    }

    # 6. Higienizar pasta locale/translations (manter somente arquivos .qm)
    $localeDir = Join-Path $destDir "locale"
    if (Test-Path $localeDir) {
        Get-ChildItem -Path $localeDir -File | Where-Object { $_.Extension -ine ".qm" } | ForEach-Object {
            Remove-Item -Path $_.FullName -Force
        }
    }

    # 7. Validar diretórios autorizados na raiz da distribuição
    $allowedRootDirs = @(
        "assets", "generic", "iconengines", "imageformats",
        "locale", "networkinformation", "platforminputcontexts",
        "platforms", "qml", "qmltooling", "sqldrivers", "styles",
        "tls", "vectorimageformats"
    )

    Get-ChildItem -Path $destDir -Directory | ForEach-Object {
        if ($_.Name -notin $allowedRootDirs) {
            Write-Warning "Removendo pasta não autorizada na distribuição limpa: $($_.FullName)"
            Remove-Item -Path $_.FullName -Recurse -Force
        }
    }

    # 8. Validação de integridade dos componentes essenciais
    $criticalFiles = @(
        (Join-Path $destDir "Qt6Core.dll"),
        (Join-Path $destDir "Qt6Gui.dll"),
        (Join-Path $destDir "Qt6Widgets.dll"),
        (Join-Path $destDir "Linceu_Lighthouse.exe"),
        (Join-Path $destDir "linceu-lighthouse-cli.exe"),
        (Join-Path $destDir "platforms\qwindows.dll")
    )

    foreach ($crit in $criticalFiles) {
        if (-not (Test-Path $crit)) {
            Write-Error "FALHA DE INTEGRIDADE: Arquivo essencial não encontrado na distribuição: $crit"
        }
    }

    # 9. Coletar estatísticas da distribuição
    $allFiles = @(Get-ChildItem -Path $destDir -Recurse -File)
    $totalSize = ($allFiles | Measure-Object -Property Length -Sum).Sum
    $sizeMB = [math]::Round($totalSize / 1MB, 2)
    $executables = @($allFiles | Where-Object { $_.Extension -eq '.exe' } | ForEach-Object { $_.Name }) -join ', '

    Write-Host "  -> Sucesso! Distribuição limpa e verificada: $($allFiles.Count) arquivos ($sizeMB MB)" -ForegroundColor Green
    Write-Host "  -> Executáveis: $executables" -ForegroundColor Green

    $results += [PSCustomObject]@{
        Distribuicao = $distName
        Caminho = $destDir
        Arquivos = $allFiles.Count
        TamanhoMB = $sizeMB
        Executaveis = $executables
    }
}

Write-Host ""
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host " RESUMO DAS DISTRIBUIÇÕES HIGIENIZADAS EM $TargetBaseDir " -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
$results | Format-Table -AutoSize
