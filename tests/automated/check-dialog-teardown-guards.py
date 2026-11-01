#!/usr/bin/env python3
"""Check legacy dialog convenience APIs fail closed during Xt teardown."""

from pathlib import Path


root = Path(__file__).resolve().parents[2]


def body(path: str, name: str, next_name: str) -> str:
    source = (root / path).read_text(encoding="utf-8")
    start = source.index(name)
    return source[start : source.index(next_name, start)]


for path, current, following in (
    ("lib/Xm/FileSB.c", "XmFileSelectionBoxGetChild(", "XmFileSelectionDoSearch("),
    ("lib/Xm/FileSB.c", "XmFileSelectionDoSearch(", "XmCreateFileSelectionBox("),
    ("lib/Xm/SelectioB.c", "XmSelectionBoxGetChild(", "XmCreateSelectionBox("),
    ("lib/Xm/MessageB.c", "XmMessageBoxGetChild(", "GetMessageString("),
    ("lib/Xm/Command.c", "XmCommandGetChild(", "XmCommandSetValue("),
    ("lib/Xm/Command.c", "XmCommandSetValue(", "XmCommandAppendValue("),
    ("lib/Xm/Command.c", "XmCommandAppendValue(", "XmCommandError("),
    ("lib/Xm/Command.c", "XmCommandError(", "XmCreateCommandDialog("),
):
    region = body(path, current, following)
    assert "being_destroyed" in region, current

append = body("lib/Xm/Command.c", "XmCommandAppendValue(", "XmCommandError(")
empty_return = append.index("XmeWarning( (Widget) w, WARNING4)")
assert append.index("XtFree(strNew)", empty_return) > empty_return

print("PASS legacy dialog convenience APIs guard teardown and free empty input")
