param(
    [string]$Ref = "HEAD"
)

$ErrorActionPreference = "Stop"

$diff = git diff --unified=0 $Ref -- '*.cpp' '*.h'
$file = $null
$bad = @()

foreach($line in $diff) {
    if($line -match '^\+\+\+ b/(.+)$') {
        $file = $Matches[1]
        continue
    }

    if($line -notmatch '^\+[^+]') {
        continue
    }

    $code = $line.Substring(1)
    if($code -notmatch '\.field\s*\(') {
        continue
    }

    $commaCount = ([regex]::Matches($code, ',')).Count
    if($commaCount -lt 2) {
        continue
    }

    if($code -match 'SAVE_COMPAT_OPTIONAL_FIELD') {
        continue
    }

    $bad += "${file}: $code"
}

if($bad.Count -gt 0) {
    Write-Host "New optional save fields need explicit review." -ForegroundColor Red
    Write-Host "Add SAVE_COMPAT_OPTIONAL_FIELD on the same line after checking field order/scope." -ForegroundColor Red
    Write-Host ""
    $bad | ForEach-Object { Write-Host $_ }
    exit 1
}

Write-Host "Save compat check passed."
