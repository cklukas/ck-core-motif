#!/usr/bin/env python3
"""Check that IconButton redisplay cannot wrap tiny widget dimensions."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "IconButton.c"
text = source.read_text(encoding="utf-8")

required = [
    "if ((int)iw->core.width > (2 * dx))",
    "width = (int)iw->core.width - (2 * dx);",
    "if ((int)iw->core.height > (2 * dy))",
    "height = (int)iw->core.height - (2 * dy);",
    "if (width > 0 && height > 0 && topgc != NULL && bottomgc != NULL)",
]
missing = [fragment for fragment in required if fragment not in text]
if missing:
    raise SystemExit("IconButton redisplay guard missing: " + ", ".join(missing))

redisplay = text[text.index("static void\nRedisplay"):text.index("/*\tFunction Name: SetValuesAlmost")]
if "width = iw->core.width - (2 * dx);" in redisplay:
    raise SystemExit("IconButton still subtracts unsigned width before conversion")
if "height = iw->core.height - (2 * dy);" in redisplay:
    raise SystemExit("IconButton still subtracts unsigned height before conversion")

print("IconButton redisplay dimension and shadow guards are present")
