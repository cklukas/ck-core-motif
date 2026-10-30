#!/usr/bin/env python3
"""Check MessageBox synthetic getters validate widget and output pointers."""

from pathlib import Path


source = Path("lib/Xm/MessageB.c").read_text()
for function in (
    "GetMessageString",
    "GetSymbolPixmap",
    "GetOkLabelString",
    "GetCancelLabelString",
    "GetHelpLabelString",
):
    start = source.rindex(function + "(")
    end = source.index("\n}", start)
    body = source[start:end]
    assert "wid == NULL || value == NULL" in body, function

print("MessageBox synthetic getters validate inputs")
