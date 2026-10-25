#!/usr/bin/env python3
"""Check FileSB filter-mask update handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
allocation = "maskString = XtMalloc((Cardinal)(len + patternLen + 1)) ;"
guard = "if(    maskString != NULL    )"
assert allocation in source
assert "patternLen == SIZE_MAX" in source
assert "len > SIZE_MAX - patternLen - 1" in source
assert "len + patternLen + 1 > UINT_MAX" in source
assert source.index(guard) > source.index(allocation)
guarded = source[source.index(allocation):source.index("XtFree( patternString) ;", source.index(allocation))]
assert "strcpy( maskString, dirString)" in guarded
assert "XmTextFieldSetString( FS_FilterText( fs), maskString)" in guarded
print("PASS FileSB filter-mask update handles allocation failure")
