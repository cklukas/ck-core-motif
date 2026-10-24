#!/usr/bin/env python3
"""Check Mrm byte-swap diagnostics use bounded formatting."""

from pathlib import Path
import re


source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIswap.c"
text = source.read_text(encoding="utf-8")
if re.search(r"^\s*sprintf\s*\(\s*err_msg", text, re.MULTILINE):
    raise SystemExit("unbounded Mrm byte-swap diagnostic remains")
formatted = len(re.findall(r"snprintf\s*\(\s*err_msg\s*,\s*sizeof\s*\(err_msg\)", text))
if formatted != 3:
    raise SystemExit(f"expected three bounded byte-swap diagnostics, found {formatted}")

print("Mrm byte-swap diagnostics use bounded formatting")
