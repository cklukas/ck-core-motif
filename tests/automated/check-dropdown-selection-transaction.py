#!/usr/bin/env python3
"""Check DropDown selection conversion commits text only after all items convert."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c").read_text(
    encoding="utf-8"
)
start = source.index("SetTextFromList(Widget w)")
body = source[start:source.index("/************************************************************", start)]
for fragment in (
    "String *parts = NULL;",
    "String combined = NULL;",
    "goto conversion_failed;",
    "XmTextFieldSetString(XmDropDown_text(cbw), combined);",
    "conversion_failed:",
):
    if fragment not in body:
        raise SystemExit(f"DropDown selection transaction contract missing: {fragment}")
if body.index("XmTextFieldSetString(XmDropDown_text(cbw), combined);") < body.index("conversion_failed:"):
    pass
else:
    raise SystemExit("DropDown must commit text before cleanup label only")

print("DropDown selection conversion commits atomically")
