#!/usr/bin/env python3
"""Check Mrm header-version diagnostics use bounded formatting."""

from pathlib import Path
import re


source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIheader.c"
text = source.read_text(encoding="utf-8")
start = text.index("Idb__HDR_GetHeader (IDBFile")
end = text.index("\n}\n", start)
region = text[start:end]
if re.search(r"^\s*sprintf\s*\(\s*errmsg\s*,", region, re.MULTILINE):
    raise SystemExit("unbounded Mrm header diagnostic remains")
if region.count("snprintf(errmsg, sizeof(errmsg)") + region.count("snprintf (errmsg, sizeof(errmsg)") != 3:
    raise SystemExit("expected three bounded Mrm header diagnostics")

print("Mrm header-version diagnostics use bounded formatting")
