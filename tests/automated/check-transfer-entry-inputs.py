#!/usr/bin/env python3
"""Check selection/drag transfer entry points validate widget handles first."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text(encoding="utf-8")
functions = ("XmeSecondaryTransfer", "XmeDragSource", "XmePrimarySink",
             "XmeNamedSink", "XmeSecondarySink", "XmeClipboardSink",
             "XmeDropSink", "XmeStandardConvert")
for function in functions:
    start = source.index(function)
    end = source.find("\n}", start)
    body = source[start:end]
    assert "w == NULL" in body, function
    assert "return" in body, function
print("PASS transfer source/sink entry points validate inputs")
