#!/usr/bin/env python3
"""Check Mrm widget conversion diagnostics use bounded formatting."""

from pathlib import Path
import re


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
if re.search(r"\b(?:v?sprintf|strcpy)\s*\(\s*err_msg\s*[,)]", text):
    raise SystemExit("unbounded Mrm conversion diagnostic formatting remains")
formatted = len(re.findall(
    r"snprintf\s*\(\s*err_msg\s*,\s*sizeof\s*\(\s*err_msg\s*\)", text
))
if formatted == 0:
    raise SystemExit("no bounded Mrm conversion diagnostics found")

print("Mrm widget conversion diagnostics use bounded formatting")
