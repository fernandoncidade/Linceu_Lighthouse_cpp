[CmdletBinding(SupportsShouldProcess = $true, ConfirmImpact = 'Low')]
param(
    [string[]]$TargetDir
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Normalize-Path {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path
    )

    return [System.IO.Path]::GetFullPath($Path).TrimEnd(
        [System.IO.Path]::DirectorySeparatorChar,
        [System.IO.Path]::AltDirectorySeparatorChar
    )
}

$scriptDir = if ($PSScriptRoot) {
    $PSScriptRoot
} elseif ($PSCommandPath) {
    Split-Path -Parent $PSCommandPath
} elseif ($MyInvocation.MyCommand.Path) {
    Split-Path -Parent $MyInvocation.MyCommand.Path
} else {
    throw 'Nao foi possivel determinar o diretorio do script.'
}

$projectRoot = Normalize-Path (Split-Path -Parent $scriptDir)
$buildRoot = Normalize-Path (Join-Path $projectRoot 'build')

$allowedDirectories = @(
    $buildRoot,
    (Join-Path $buildRoot 'build_mocks'),
    (Join-Path $buildRoot 'build_qt6_mingw'),
    (Join-Path $buildRoot 'build_qt6_msvc')
) | ForEach-Object {
    Normalize-Path $_
}

if (-not $TargetDir -or $TargetDir.Count -eq 0) {
    $TargetDir = @($buildRoot)
}

foreach ($directory in $TargetDir) {
    if ([string]::IsNullOrWhiteSpace($directory)) {
        continue
    }

    $resolvedDirectory = Normalize-Path $directory

    $isAllowed = ($allowedDirectories -contains $resolvedDirectory) -or ($resolvedDirectory.StartsWith($buildRoot + [System.IO.Path]::DirectorySeparatorChar))
    if (-not $isAllowed) {
        throw "Por seguranca, o script so limpa estes diretorios: $($allowedDirectories -join ', '). Recebido: $resolvedDirectory"
    }

    if (-not (Test-Path -LiteralPath $resolvedDirectory -PathType Container)) {
        Write-Host "[IGNORADO] Diretorio nao encontrado: $resolvedDirectory"
        continue
    }

    if ($PSCmdlet.ShouldProcess($resolvedDirectory, 'Excluir diretorio regeneravel completo')) {
        Remove-Item -LiteralPath $resolvedDirectory -Recurse -Force -Confirm:$false
        Write-Host "[REMOVIDO] $resolvedDirectory"
    }
}

Write-Host 'Limpeza concluida.'
Write-Host 'Use -WhatIf para simular antes de remover de fato.'
