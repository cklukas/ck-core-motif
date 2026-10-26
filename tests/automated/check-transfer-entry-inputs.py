#!/usr/bin/env python3
"""Check selection/drag transfer entry points validate widget handles first."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text(encoding="utf-8")
checks = {
    "XmeSecondaryTransfer": "if (w == NULL)\n    return;",
    "XmeDragSource": "if (w == NULL)\n    return NULL;",
    "XmePrimarySink": "if (w == NULL)\n    return False;",
    "XmeNamedSink": "if (w == NULL || sel == None)\n    return False;",
    "XmeSecondarySink": "if (w == NULL)\n    return False;",
    "XmeClipboardSink": "if (w == NULL)\n    return False;",
    "XmeDropSink": "if (w == NULL)\n    return;",
    "XmeStandardConvert": "if (w == NULL || cs == NULL)\n    return;",
}
for function, guard in checks.items():
    start = source.index(function)
    end = source.find("\n}", start)
    assert guard in source[start:end], function
print("PASS transfer source/sink entry points validate inputs")
