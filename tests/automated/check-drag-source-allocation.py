#!/usr/bin/env python3
"""Check XmeDragSource argument and target failure cleanup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeDragSource(")
end = source.index("\nvoid\nXmeDragSource", start) if "\nvoid\nXmeDragSource" in source[start:] else source.index("\n/****************************************************************/\n/* Destination section", start)
region = source[start:end]

assert "targets = NULL;" in region
assert "in_arg_count > (Cardinal)(INT_MAX - 10)" in region
assert "(in_arg_count != 0 && in_args == NULL)" in region
assert "(size_t)(in_arg_count + 10) > (size_t)UINT_MAX / sizeof(*args)" in region
assert "args = (Arg *)XtMalloc(sizeof(*args) * (size_t)arg_count);" in region
assert "if (args == NULL)" in region
assert "XtFree((char*) targets);" in region

print("PASS drag-source allocation and failure cleanup checks")
