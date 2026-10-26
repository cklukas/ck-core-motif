#!/usr/bin/env python3
"""Check that XIM preedit lengths are validated before narrowing/arithmetic."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void\nPreeditDraw(")
end = text.index("\n}\n", start)
region = text[start:end]

if "int insert_length=0;" not in region:
    raise SystemExit("preedit length is still stored in a narrow type")
if "call_data->text->length < 0" not in region:
    raise SystemExit("negative XIM preedit lengths are not rejected")
if "unsigned short insert_length" in region:
    raise SystemExit("preedit length retains the old 16-bit storage")

print("XIM preedit length is validated at callback width")
