#!/usr/bin/env python3
"""Check tab-list parsing and insertion failure handling."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
start = text.index("static Boolean \nGetNextTab(")
body = text[start : start + 800]
if "s == NULL || *s == NULL" not in body:
    raise SystemExit("tab parser input guard is missing")
start = text.rfind("CvtStringToXmTabList(")
body = text[start : start + 1900]
for needle in ("dpy == NULL", "from == NULL", "to == NULL",
               "new_tl = XmTabListInsertTabs", "new_tl == NULL"):
    if needle not in body:
        raise SystemExit(f"tab-list converter handling is missing: {needle}")

print("tab-list converter guards and rollback present")
