# Press keys in a window (PCSX2: its pad 1 is bound to the keyboard too, Square = J),
# then give the focus back. Windows only. Used by ttt_capture_theater.py.
param([int]$ProcId, [string]$Keys = '74', [int]$HoldMs = 300, [int]$GapMs = 450)
Add-Type @"
using System; using System.Runtime.InteropServices;
public class K {
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int c);
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte scan, uint flags, UIntPtr extra);
  [DllImport("user32.dll")] public static extern uint MapVirtualKey(uint code, uint type);
}
"@
$p = Get-Process -Id $ProcId
$h = $p.MainWindowHandle
if ($h -eq [IntPtr]::Zero) { Write-Output "no window"; exit 1 }
$prev = [K]::GetForegroundWindow()
# Windows refuses SetForegroundWindow to a process that is not in the
# foreground; a tap of Alt first lifts that lock.
for ($try = 0; $try -lt 5 -and [K]::GetForegroundWindow() -ne $h; $try++) {
  [K]::keybd_event(0x12, 0, 0, [UIntPtr]::Zero); [K]::keybd_event(0x12, 0, 2, [UIntPtr]::Zero)
  [K]::ShowWindow($h, 9) | Out-Null
  [K]::SetForegroundWindow($h) | Out-Null
  Start-Sleep -Milliseconds 300
}
if ([K]::GetForegroundWindow() -ne $h) { Write-Output "not focused"; exit 2 }
# Keys: virtual key codes, comma separated (arrows 37-40, J 74, K 75, L 76, I 73, Return 13).
foreach ($k in $Keys.Split(',')) {
  $vk = [byte][int]$k
  $sc = [byte][K]::MapVirtualKey($vk, 0)
  $ext = if ($vk -ge 37 -and $vk -le 40) { 1 } else { 0 }      # the arrows are extended keys
  [K]::keybd_event($vk, $sc, $ext, [UIntPtr]::Zero)
  Start-Sleep -Milliseconds $HoldMs
  [K]::keybd_event($vk, $sc, 2 + $ext, [UIntPtr]::Zero)
  Start-Sleep -Milliseconds $GapMs
}
[K]::SetForegroundWindow($prev) | Out-Null
Write-Output "pressed"
