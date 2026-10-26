#!/usr/bin/env python3
"""Check XmeDropSink argument-array validation and allocation handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeDropSink(")
end = source.index("\nXmTransferDone(", start)
region = source[start:end]

assert "in_arg_count > (Cardinal)(INT_MAX - 2)" in region
assert "(in_arg_count != 0 && in_args == NULL)" in region
assert "(size_t)(in_arg_count + 2) > (size_t)UINT_MAX / sizeof(*args)" in region
assert "args = (Arg *)XtMalloc(sizeof(*args) * (size_t)arg_count);" in region
assert "if (args == NULL)" in region
assert "_XmAppUnlock(app);" in region

print("PASS drop-sink allocation and argument checks")
