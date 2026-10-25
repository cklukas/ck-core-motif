#!/usr/bin/env python3
"""Check tab-list mutation APIs reject out-of-range positions."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c").read_text()

assert source.count("position_list[i] >= _XmTabLCount(tl)") == 2
assert source.count("return(oldlist);") >= 2

print("tab-list mutation positions are range-checked")
