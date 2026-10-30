#!/usr/bin/env python3
"""Check RowColumn synthetic string getters validate callback inputs."""

from pathlib import Path


source = Path("lib/Xm/RowColumn.c").read_text()
for function in (
    "GetMnemonicCharSet",
    "GetMenuAccelerator",
    "GetMenuPost",
    "GetLabelString",
    "GetTearOffTitle",
):
    start = source.rindex(function + "(")
    end = source.index("\n}", start)
    body = source[start:end]
    assert "value == NULL" in body, function
    assert "wid == NULL" in body, function
    assert "XmIsRowColumn(wid)" in body, function

print("RowColumn synthetic string getters validate widget and output pointers")
