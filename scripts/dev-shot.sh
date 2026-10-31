#!/bin/sh
out="${1:-dev-shot.png}"
settle="${2:-2500}"
dir="$(cd "$(dirname "$0")" && pwd)"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "$dir/dev-shot.ps1" -OutPath "$out" -SettleMs "$settle"
