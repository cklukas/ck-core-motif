#!/usr/bin/env python3
"""Check Badge renders supplied compound labels without flattening them."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Badge.c").read_text(
    encoding="utf-8"
)
body = source[source.index("static XmString display_label"):source.rindex("static void natural_size")]
if "XmStringUnparse" in body:
    raise SystemExit("Badge compound labels must not be flattened before drawing")
if "badge->badge.count < 0" not in body:
    raise SystemExit("Badge numeric labels must retain their generated-string path")
if "return XmStringCopy(badge->badge.label)" not in body or "snprintf(text" not in body:
    raise SystemExit("Badge must copy the original XmString for supplied labels")

print("Badge preserves compound labels for Unicode-safe drawing")
