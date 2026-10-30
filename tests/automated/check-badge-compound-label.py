#!/usr/bin/env python3
"""Check Badge renders supplied compound labels without flattening them."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Badge.c").read_text(
    encoding="utf-8"
)
start = source.index("static void text_value")
end = source.index("static Boolean SetValues", start)
body = source[start:end]
if "XmStringUnparse" in body:
    raise SystemExit("Badge compound labels must not be flattened before drawing")
if "badge->badge.count >= 0" not in body:
    raise SystemExit("Badge numeric labels must retain their generated-string path")
if "badge->badge.count >= 0) ? XmStringCreateLocalized(text) :" not in body:
    raise SystemExit("Badge must draw the original XmString for supplied labels")

print("Badge preserves compound labels for Unicode-safe drawing")
