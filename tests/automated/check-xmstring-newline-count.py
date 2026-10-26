#!/usr/bin/env python3
"""Check that XmString newline insertion cannot overflow entry count."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c"
text = source.read_text(encoding="utf-8")
start = text.index("static Boolean\nnew_line(")
end = text.index("\n}\n", start)
region = text[start:end]

if "if (lc < 0 || lc == INT_MAX)" not in region:
    raise SystemExit("XmString newline entry-count guard missing")

print("XmString newline insertion bounds entry count")
