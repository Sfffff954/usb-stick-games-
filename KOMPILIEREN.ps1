# NetPanel Games - Automatisch kompilieren
# Rechtsklick -> "Mit PowerShell ausfuhren"
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass -Force

$dir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $dir
Write-Host "=== NetPanel Games Compiler ===" -ForegroundColor Cyan

# Schon kompiliert?
if (Test-Path "$dir\NetPanelGames.exe") {
    Write-Host "Bereits kompiliert! Starte..." -ForegroundColor Green
    Start-Process "$dir\NetPanelGames.exe"; exit
}

# GCC suchen
$gcc = $null
$try = @(
    "C:\msys64\mingw64\bin\gcc.exe",
    "C:\msys64\mingw32\bin\gcc.exe",
    "C:\MinGW\bin\gcc.exe",
    "C:\TDM-GCC-64\bin\gcc.exe"
)
foreach ($p in $try) { if (Test-Path $p) { $gcc = $p; break } }
if (!$gcc) { try { $gcc = (Get-Command gcc -EA Stop).Source } catch {} }

if (!$gcc) {
    Write-Host "GCC nicht gefunden. Installiere winlibs (klein, schnell)..." -ForegroundColor Yellow
    $url = "https://github.com/brechtsanders/winlibs_mingw/releases/download/13.2.0posix-17.0.6-11.0.1-msvcrt-r5/winlibs-x86_64-posix-seh-gcc-13.2.0-mingw-w64msvcrt-11.0.1-r5.7z"
    $tmp = "$env:TEMP\winlibs.7z"
    $ext = "C:\winlibs"
    Write-Host "Lade herunter..." -ForegroundColor Yellow
    try {
        (New-Object Net.WebClient).DownloadFile($url, $tmp)
        # extract with built-in Windows or 7zip
        if (Get-Command 7z -EA SilentlyContinue) {
            7z x $tmp -o"$ext" -y | Out-Null
        } else {
            # fallback: use tar (available in Win10+)
            New-Item -ItemType Directory -Force -Path $ext | Out-Null
            tar -xf $tmp -C $ext 2>$null
        }
        $gcc = (Get-ChildItem "$ext" -Recurse -Filter "gcc.exe" | Select -First 1).FullName
    } catch {
        Write-Host "Download fehlgeschlagen." -ForegroundColor Red
        Write-Host ""
        Write-Host "Bitte manuell:" -ForegroundColor White
        Write-Host "1. https://winlibs.com -> Win64 -> MSVCRT -> .zip herunterladen" -ForegroundColor White
        Write-Host "2. Entpacken nach C:\winlibs" -ForegroundColor White  
        Write-Host "3. Dieses Script nochmal starten" -ForegroundColor White
        Read-Host "Enter"
        exit
    }
}

if ($gcc) {
    Write-Host "GCC: $gcc" -ForegroundColor Green
    Write-Host "Kompiliere..." -ForegroundColor Yellow
    $gdir = Split-Path $gcc
    $env:PATH = "$gdir;$env:PATH"
    & $gcc "$dir\games.c" -o "$dir\NetPanelGames.exe" -lgdi32 -lcomctl32 -lwinmm -mwindows -O2 -lm -s
    if (Test-Path "$dir\NetPanelGames.exe") {
        Write-Host "Fertig! Starte..." -ForegroundColor Green
        Start-Process "$dir\NetPanelGames.exe"
    } else {
        Write-Host "Fehler beim Kompilieren!" -ForegroundColor Red
        Read-Host "Enter"
    }
} else {
    Write-Host "Konnte GCC nicht installieren." -ForegroundColor Red
    Read-Host "Enter"
}
