#!/usr/bin/env python3
"""Check that TextField overstrike recovery keeps position width."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void\nPreeditDraw(")
end = text.index("\n}\n", start)
region = text[start:end]

if "XmTextPosition recover_len=0;" not in region:
    raise SystemExit("TextField recovery span is still narrow")
if "size_t i;" not in region:
    raise SystemExit("TextField recovery index is still narrow")
if "int recover_len=0;" in region:
    raise SystemExit("TextField retains int recovery span")

print("TextField preedit recovery preserves position width")
