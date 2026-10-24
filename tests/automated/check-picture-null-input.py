#!/usr/bin/env python3
"""Check that picture parsing rejects a missing specification."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Picture.c"
text = source.read_text(encoding="utf-8")

if "if (input == NULL)" not in text:
    raise SystemExit("picture parser null-input guard is missing")
if "return NULL;" not in text:
    raise SystemExit("picture parser null input does not return failure")

print("picture parser rejects null input")
