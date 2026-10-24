#!/usr/bin/env python3
"""Check Mrm widget-reference diagnostics use bounded formatting."""

from pathlib import Path
import re


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwrefs.c"
text = source.read_text(encoding="utf-8")
if re.search(r"^\s*sprintf\s*\(\s*errmsg", text, re.MULTILINE):
    raise SystemExit("unbounded Mrm widget-reference diagnostic remains")
formatted = len(
    re.findall(r"snprintf\s*\(\s*errmsg\s*,\s*sizeof\s*\(errmsg\)", text)
)
if formatted != 2:
    raise SystemExit(f"expected two bounded widget-reference diagnostics, found {formatted}")

print("Mrm widget-reference diagnostics use bounded formatting")
