#!/usr/bin/env python3
"""Check icon-directory cache copies use a bounded Xt allocation width."""

from pathlib import Path


source = Path("lib/Xm/IconFile.c").read_text()

assert "dirNameLen == SIZE_MAX || dirNameLen >= UINT_MAX" in source
assert "XtMalloc((Cardinal)(dirNameLen + 1))" in source

print("icon-directory allocation width is guarded")
