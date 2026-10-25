#!/usr/bin/env python3
"""Check menu button border clears for undersized widgets."""
from pathlib import Path

root = Path(__file__).resolve().parents[2] / "lib" / "Xm"
files = {
    "CascadeB.c": ("Disarm",),
    "PushB.c": ("Leave",),
    "ToggleB.c": ("Leave", "BorderUnhighlight"),
}

for filename, names in files.items():
    text = (root / filename).read_text(encoding="utf-8")
    for name in names:
        marker = "static void \n" + name
        start = text.rindex(marker)
        end = text.find("\f", start)
        body = text[start:] if end < 0 else text[start:end]
        if "XmeClearBorder" not in body:
            raise SystemExit(f"{filename}:{name} has no border clear to check")
        widget = "cb" if filename == "CascadeB.c" else "pb" if filename == "PushB.c" else "w" if name == "Leave" else "tb"
        if f"(int){widget}->core.width > 2 * (int)" not in body:
            raise SystemExit(f"{filename}:{name} lacks a positive width guard")
        if f"(int){widget}->core.height > 2 * (int)" not in body:
            raise SystemExit(f"{filename}:{name} lacks a positive height guard")

print("CascadeButton, PushButton, and ToggleButton menu border guards are present")
