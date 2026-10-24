#!/usr/bin/env python3
"""Check Mrm set-values fetching handles setup allocations and zero args."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwvalues.c").read_text(encoding="utf-8")
start = source.index("\nUrmFetchSetValues (")
region = source[start:]
zero = region.index("if (num_args == 0)")
args_check = region.index("if (num_args != 0 && args == NULL)")
assert args_check < zero
loc_alloc = region.index("locargs = (ArgList) XtMalloc")
assert region.index("if (locargs == NULL)", loc_alloc) < region.index("UrmPlistInit", loc_alloc)
plist = region.index("UrmPlistInit (num_args, &ptrlist)")
assert "!= MrmSUCCESS" in region[plist:plist + 100]
res_alloc = region.index("resptr = (RGMResourceDescPtr) XtMalloc")
assert "if (resptr == NULL)" in region[res_alloc:res_alloc + 240]
font_alloc = region.index("RGMFontListPtr fontlist", res_alloc)
assert "if (fontlist == NULL)" in region[font_alloc:font_alloc + 360]
print("Mrm set-values fetching validates setup and font-list allocations")
