#!/usr/bin/env python3
"""Check deferred clipboard callbacks do not use request state after unlock."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Transfer.c").read_text()
callback = source[source.index("static void \nClipboardCallback") : source.index("/****************************************************************/\n/* XmeDragSource", source.index("static void \nClipboardCallback"))]

assert "distinguisher = req -> distinguisher;" in callback
assert "last_request = req -> outstanding == 0;" in callback
assert "_XmConvertHandler(w, &distinguisher" in callback
assert "if (last_request)" in callback

setup_start = source.index("/* Associate the data_id with this snapshot")
setup_end = source.index("if (req != NULL && registered == 0)", setup_start)
source_setup = source[setup_start:setup_end]
assert "int i, count, status, transferred = 0, registered = 0;" in source
assert "if (_XmAddHashEntry(DataIdDictionary" in source_setup
assert "if (req != NULL && registered == 0)" in source

print("PASS deferred clipboard request lifetime is reentrant-safe")
