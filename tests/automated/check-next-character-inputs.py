#!/usr/bin/env python3
"""Check that XmeGetNextCharacter validates callback input pointers."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c"
text = source.read_text(encoding="utf-8")
needle = """  if (in_out == NULL || *in_out == NULL || str_include == NULL ||
      pattern_length < 0)
    return XmINSERT;
"""
if needle not in text:
    raise SystemExit("next-character input guard is missing")

print("XmeGetNextCharacter input guards present")
