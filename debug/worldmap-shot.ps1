# Launch Olympus straight to the world map, render a screenshot, exit.
param(
  [string]$Exe = "G:\games\eZeus\olympus\Bin\Olympus.exe",
  [string]$Out = "$PSScriptRoot\worldmap.png",
  [string]$Log = "$PSScriptRoot\worldmap.log",
  [string]$City = "",
  [int]$WaitSec = 11
)

$Exe = (Resolve-Path $Exe).Path
$ExeDir = Split-Path -Parent $Exe
$LogOut = "$Log.out"
$LogErr = "$Log.err"
Remove-Item -ErrorAction SilentlyContinue $Log, $LogOut, $LogErr
Add-Content -Path $Log -Value "worldmap-shot: exe=$Exe"
Add-Content -Path $Log -Value "worldmap-shot: out=$Out"

Add-Type -AssemblyName System.Drawing

$exeArgs = @("--dev-world-map-shot", $Out)
if($City -ne "") {
  $exeArgs += @("--dev-click-city", $City)
  Add-Content -Path $Log -Value "worldmap-shot: city=$City"
}
$p = Start-Process `
  -FilePath $Exe `
  -WorkingDirectory $ExeDir `
  -ArgumentList $exeArgs `
  -WindowStyle Hidden `
  -RedirectStandardOutput $LogOut `
  -RedirectStandardError $LogErr `
  -PassThru
$exitDeadline = (Get-Date).AddSeconds($WaitSec)
while(!$p.HasExited -and (Get-Date) -lt $exitDeadline) {
  Start-Sleep -Milliseconds 250
  $p.Refresh()
}
if(!$p.HasExited) {
  Stop-Process -Id $p.Id -Force
  Get-Content -ErrorAction SilentlyContinue $LogOut, $LogErr | Add-Content $Log
  Add-Content -Path $Log -Value "worldmap-shot: timeout after ${WaitSec}s"
  Get-Content -ErrorAction SilentlyContinue $Log
  throw "Olympus did not finish rendering within $WaitSec seconds."
}
$p.Refresh()
Get-Content -ErrorAction SilentlyContinue $LogOut, $LogErr | Add-Content $Log
$exitCode = $p.ExitCode
if($null -eq $exitCode) {
  $exitCode = 0
}
Add-Content -Path $Log -Value "worldmap-shot: exit=$exitCode"
if($exitCode -ne 0) {
  Get-Content -ErrorAction SilentlyContinue $Log
  throw "Olympus exited with code $exitCode."
}
if(!(Test-Path $Out)) {
  Add-Content -Path $Log -Value "worldmap-shot: missing output"
  Get-Content -ErrorAction SilentlyContinue $Log
  throw "Olympus exited without saving $Out."
}
$img = [System.Drawing.Image]::FromFile($Out)
$savedW = $img.Width
$savedH = $img.Height
$img.Dispose()
Add-Content -Path $Log -Value "worldmap-shot: saved $Out (${savedW}x${savedH})"
Get-Content -ErrorAction SilentlyContinue $Log
Write-Output "saved $Out (${savedW}x${savedH})"
