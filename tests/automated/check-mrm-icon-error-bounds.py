#!/usr/bin/env python3
"""Check Mrm icon diagnostics use bounded fixed-buffer formatting."""

from pathlib import Path
import re


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmicon.c"
text = source.read_text(encoding="utf-8")
if re.search(r"^\s*sprintf\s*\(\s*err_msg", text, re.MULTILINE):
    raise SystemExit("unbounded Mrm icon error formatting remains")
formatted = text.count("snprintf (err_msg, sizeof(err_msg),")
if formatted != 12:
    raise SystemExit(f"expected 12 bounded icon diagnostics, found {formatted}")

print("Mrm icon diagnostics use bounded formatting")
