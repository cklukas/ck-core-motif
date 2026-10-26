#!/usr/bin/env python3
"""Check clipboard paste buffer sizing and allocation."""

from pathlib import Path


source = Path("lib/Xm/DataF.c").read_text(encoding="utf-8")

assert "if (length > UINT_MAX)" in source
assert "buffer = XtMalloc((Cardinal)length);" in source
assert "if (buffer == NULL)" in source

print("DataField paste-buffer checks passed")
