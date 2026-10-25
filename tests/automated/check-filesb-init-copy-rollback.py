#!/usr/bin/env python3
"""Check FileSelectionBox initialization copies are failure-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
start = source.index("static void \nInitialize(")
title = source.index("    if( FS_NoMatchString( new_w)", start)
end = source.index("\n    if(    !FS_QualifySearchDataProc", title)
region = source[title:end]

assert "replacement = XmStringCopy( FS_NoMatchString( new_w));" in region
assert "if (replacement != NULL)" in region
assert "if (searchData.mask == NULL)" in region
assert 'XmStringCreate("*", XmFONTLIST_DEFAULT_TAG)' in region
assert "if (searchData.mask != NULL)" in region
assert "if (searchData.dir != NULL)" in region
assert "if (searchData.pattern != NULL)" in region
print("FileSelectionBox initialization copies handle allocation failure")
