#!/usr/bin/env python3
"""Check Command convenience APIs reject non-Command widgets."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Command.c").read_text(
    encoding="utf-8"
)

for name, guard in (
    ("XmCommandGetChild", "if (widget == NULL || !XmIsCommand(widget))"),
    ("XmCommandSetValue", "if (widget == NULL || !XmIsCommand(widget) || value == NULL)"),
    ("XmCommandAppendValue", "if (widget == NULL || !XmIsCommand(widget) || value == NULL)"),
    ("XmCommandError", "if (widget == NULL || !XmIsCommand(widget) || error == NULL)"),
):
    start = source.index(name + "(")
    body = source[start : source.index("\n}\n", start) + 3]
    if guard not in body:
        raise SystemExit(f"{name} lacks Command class validation")
    if body.index(guard) > body.index("_XmWidgetToAppContext(widget)"):
        raise SystemExit(f"{name} validates its widget after context lookup")

print("Command convenience APIs validate widget class before access")
