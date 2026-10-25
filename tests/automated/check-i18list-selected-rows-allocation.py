#!/usr/bin/env python3
"""Guard I18List selected-row result allocation."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
start = text.index("XmI18ListGetSelectedRows(")
body = text[start:text.index("\n}\n", start) + 2]
if "ret_rows = (XmMultiListRowInfo **) XtMalloc" not in body:
    raise SystemExit("I18List selected-row allocation not found")
if "if (ret_rows == NULL)" not in body or "return NULL;" not in body:
    raise SystemExit("I18List selected-row allocation failure is not handled")
if body.index("if (ret_rows == NULL)") > body.index("ret_rows[i] = NULL"):
    raise SystemExit("I18List writes result terminator before allocation check")

print("I18List selected-row allocation checks passed")
