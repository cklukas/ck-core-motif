#!/usr/bin/env python3
"""Check that XIM rendition feedback iteration keeps the source width."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void\nPreeditSetRendition(")
end = text.index("\n}\n", start)
region = text[start:end]

if "int cnt;" not in region:
    raise SystemExit("preedit feedback counter is still narrow")
if "text == NULL || text->length <= 0" not in region:
    raise SystemExit("invalid XIM feedback lengths are not rejected")
if "unsigned short cnt" in region:
    raise SystemExit("preedit feedback retains a 16-bit counter")

print("XIM preedit feedback iteration preserves callback width")
