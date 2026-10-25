#!/usr/bin/env python3
"""Check FileSB filter-mask update handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
allocation = "maskString = XtMalloc( len + strlen( patternString) + 1) ;"
guard = "if(    maskString != NULL    )"
assert allocation in source
assert source.index(guard) > source.index(allocation)
guarded = source[source.index(allocation):source.index("XtFree( patternString) ;", source.index(allocation))]
assert "strcpy( maskString, dirString)" in guarded
assert "XmTextFieldSetString( FS_FilterText( fs), maskString)" in guarded
print("PASS FileSB filter-mask update handles allocation failure")
