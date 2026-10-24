#!/usr/bin/env python3
"""Check Mrm literal/color diagnostics use bounded formatting."""

from pathlib import Path
import re


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmlread.c"
text = source.read_text(encoding="utf-8")
if re.search(r"^\s*sprintf\s*\(\s*err_msg", text, re.MULTILINE):
    raise SystemExit("unbounded Mrm literal diagnostic remains")
formatted = text.count("snprintf (err_msg, sizeof(err_msg)") + text.count(
    "snprintf(err_msg, sizeof(err_msg)"
)
if formatted != 2:
    raise SystemExit(f"expected two bounded literal diagnostics, found {formatted}")

print("Mrm literal diagnostics use bounded formatting")
