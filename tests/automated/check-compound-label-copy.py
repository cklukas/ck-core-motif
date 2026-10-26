#!/usr/bin/env python3
"""Ensure label ownership preserves compound-string segments."""

from pathlib import Path

root = Path(__file__).parents[2] / "lib/Xm"
required = {
    "StatusBar.c": "return value ? XmStringCopy(value) : XmStringCreateLocalized(\"\");",
    "Toolbar.c": "return value ? XmStringCopy(value) : XmStringCreateLocalized(\"\");",
    "Toast.c": "return source ? XmStringCopy(source) : XmStringCreateLocalized(\"\");",
    "Skeleton.c": "return value ? XmStringCopy(value) : XmStringCreateLocalized(\"\");",
    "Divider.c": "return source ? XmStringCopy(source) : XmStringCreateLocalized(\"\");",
    "Chip.c": "return source ? XmStringCopy(source) : XmStringCreateLocalized(\"\");",
}
for filename, expression in required.items():
    source = (root / filename).read_text()
    if expression not in source:
        raise SystemExit(f"{filename} does not preserve compound strings")

for filename, expressions in {
    "StatusBar.c": ("XmStringConcat(label, suffix)",),
    "Toolbar.c": ("action->shortcut && !XmStringEmpty(action->shortcut)",
                   "XmStringConcat(result, separator)",
                   "XmStringConcat(combined, shortcut)"),
}.items():
    source = (root / filename).read_text()
    for expression in expressions:
        if expression not in source:
            raise SystemExit(f"{filename} does not concatenate compound strings")

print("PASS status, toolbar, toast, and skeleton labels preserve compound strings")
