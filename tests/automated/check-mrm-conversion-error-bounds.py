#!/usr/bin/env python3
"""Check Mrm widget conversion diagnostics use bounded formatting."""

from pathlib import Path
import re


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
if re.search(r"^\s*sprintf\s*\(\s*err_msg\s*,", text, re.MULTILINE):
    raise SystemExit("unbounded Mrm conversion diagnostic formatting remains")
formatted = text.count("snprintf (err_msg, sizeof(err_msg),")
if formatted != 37:
    raise SystemExit(f"expected 37 bounded conversion diagnostics, found {formatted}")

print("Mrm widget conversion diagnostics use bounded formatting")
