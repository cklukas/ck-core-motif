#!/usr/bin/env python3
"""Check that XmeGetIconControlInfo validates all output pointers."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text(
    encoding="utf-8"
)
start = text.index("XmeGetIconControlInfo(")
body = text[start : text.index("}\n", start) + 2]
guard = (
    "if (useMaskRtn == NULL || useMultiColorIconsRtn == NULL ||\n"
    "\tuseIconFileCacheRtn == NULL)\n\treturn False;"
)
if guard not in body:
    raise SystemExit("icon-control output guard is missing")
if body.index(guard) > body.index("_XmProcessLock();"):
    raise SystemExit("icon-control outputs are checked too late")

print("icon-control output guards present")
