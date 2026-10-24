#!/usr/bin/env python3
"""Check safe ctype use when formatting representation type names."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "RepType.c"
text = source.read_text(encoding="utf-8")

if "unsigned char ch = (unsigned char)TmpStr[Index][i];" not in text:
    raise SystemExit("representation type ctype input is not unsigned")
if "if (islower(ch))" not in text or "toupper(ch)" not in text:
    raise SystemExit("representation type ctype calls are not bounded")

print("representation type ctype conversion is defined for all bytes")
