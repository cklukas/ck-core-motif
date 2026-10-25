#!/usr/bin/env python3
"""Check FileSB resource-change filter-mask allocation handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
needle = "maskString = XtMalloc( len\n                                                 + strlen( patternString) + 1) ;"
start = source.index(needle)
guard = source.index("if(    maskString != NULL    )", start)
end = source.index("XtFree( patternString) ;", guard)
assert start < guard < end
assert "XmTextFieldSetString( FS_FilterText( fsb)" in source[guard:end]
print("PASS FileSB resource-change filter mask handles allocation failure")
