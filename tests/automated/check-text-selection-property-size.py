#!/usr/bin/env python3
"""Check Text selection property exports fit Xt's allocation width."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
assert source.count("tmp_prop.nitems > (unsigned long)UINT_MAX") == 3
assert source.count("(size_t)tmp_prop.nitems : 1") == 3

print("Text selection property sizes fit Xt allocations")
