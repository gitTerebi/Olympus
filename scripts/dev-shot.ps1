param(
    [string]$OutPath = "$PSScriptRoot\..\dev-shot.png",
    [int]$SettleMs = 2500,
    [switch]$KeepOpen
)

$exe = "G:\games\eZeus\olympus\Bin\Olympus.exe"
Start-Process -FilePath $exe -WorkingDirectory (Split-Path $exe) -ArgumentList "--dev-load-recent"

Add-Type @'
using System;
using System.Runtime.InteropServices;
public class Win32 {
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern IntPtr FindWindow(string cls, string title);
  public struct RECT { public int Left, Top, Right, Bottom; }
}
'@
Add-Type -AssemblyName System.Drawing

$hwnd = [IntPtr]::Zero
$deadline = (Get-Date).AddSeconds(30)
while((Get-Date) -lt $deadline) {
    $hwnd = [Win32]::FindWindow($null, "eZeus")
    if($hwnd -ne [IntPtr]::Zero) { break }
    Start-Sleep -Milliseconds 200
}
if($hwnd -eq [IntPtr]::Zero) { Write-Error "eZeus window not found"; exit 1 }

[Win32]::SetForegroundWindow($hwnd) | Out-Null
Start-Sleep -Milliseconds $SettleMs
[Win32]::SetForegroundWindow($hwnd) | Out-Null
Start-Sleep -Milliseconds 300

$r = New-Object Win32+RECT
[Win32]::GetWindowRect($hwnd, [ref]$r) | Out-Null
$w = $r.Right - $r.Left
$h = $r.Bottom - $r.Top
$bmp = New-Object System.Drawing.Bitmap $w, $h
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.CopyFromScreen($r.Left, $r.Top, 0, 0, (New-Object System.Drawing.Size $w, $h))
$bmp.Save($OutPath)
$g.Dispose()
$bmp.Dispose()

if(-not $KeepOpen) {
    try { Stop-Process -Name Olympus -Force -Confirm:$false -ErrorAction Stop } catch {}
}
Write-Output "saved $OutPath ($w x $h)"
