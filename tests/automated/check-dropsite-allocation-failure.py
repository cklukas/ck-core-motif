#!/usr/bin/env python3
"""Ensure drop-site manager allocation failures are handled safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropSMgr.c").read_text()
start = source.index("static void \nEndUpdate(")
body = source[start:]
assert "if (dsupdate == NULL)" in body
assert "UINT_MAX / sizeof(Widget)" in body
assert "if (*children_rtn == NULL)" in body
print("PASS drop-site manager allocation failures are handled safely")
