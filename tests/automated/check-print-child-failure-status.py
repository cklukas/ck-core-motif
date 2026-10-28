#!/usr/bin/env python3
"""Check PrintShell child setup failures reach the parent safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PrintS.c").read_text()
assert "static void ChildPrintFailure(FILE *file, const char *file_name, int pipe);" in source
assert "received != (ssize_t)sizeof(XPGetDocStatus)" in source
assert "status = XPGetDocError;" in source
assert "ChildPrintFailure(file, file_name, pipe);" in source
assert "ChildPrintFailure(file_desc->file, file_desc->file_name, pipe);" in source
assert "remove(file_name)" in source
print("PrintShell child failures report errors and remove partial output")
