#!/usr/bin/env python3
"""Check ToggleButton menu enter/highlight shadow guards."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "ToggleB.c"
text = source.read_text(encoding="utf-8")

for name, widget in (("Enter", "w"), ("BorderHighlight", "tb")):
    start = text.rindex("static void \n" + name)
    end = text.find("\f", start)
    body = text[start:] if end < 0 else text[start:end]
    required = [
        f"(int){widget}->core.width > 2 * (int){widget}->primitive.highlight_thickness",
        f"(int){widget}->core.height > 2 * (int){widget}->primitive.highlight_thickness",
        f"{widget}->primitive.top_shadow_GC != NULL",
        f"{widget}->primitive.bottom_shadow_GC != NULL",
        f"(int){widget}->core.width -",
        f"(int){widget}->core.height -",
    ]
    missing = [fragment for fragment in required if fragment not in body]
    if missing:
        raise SystemExit(f"{name} guard missing: " + ", ".join(missing))

print("ToggleButton menu enter/highlight shadow guards are present")
