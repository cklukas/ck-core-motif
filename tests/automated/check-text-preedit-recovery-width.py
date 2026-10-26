#!/usr/bin/env python3
"""Check that overstrike preedit recovery keeps text positions wide."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void\nPreeditDraw(")
end = text.index("\n}\n", start)
region = text[start:end]

for required in (
    "XmTextPosition recover_len = 0;",
    "XmTextPosition i;",
    "size_t total_mb_len;",
    "total_mb_len += (size_t)char_len;",
):
    if required not in region:
        raise SystemExit(f"missing wide preedit recovery state: {required}")
if "int recover_len = 0;" in region or "int total_mb_len;" in region:
    raise SystemExit("preedit recovery retains a narrow length")

print("preedit overstrike recovery preserves text-position width")
