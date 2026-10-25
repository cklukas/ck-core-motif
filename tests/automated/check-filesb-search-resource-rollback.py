#!/usr/bin/env python3
"""Check FileSelectionBox search-resource copies are failure-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
start = source.index("if(    FS_DirMask( new_w) !=")
end = source.index("\n    if(    FS_FileTypeMask", start)
region = source[start:end]

assert "searchData.mask = XmStringCopy" in region
assert "if (searchData.mask != NULL)" in region
assert "searchData.mask_length = XmStringLength" in region
assert "resourceCopy = XmStringCopy( FS_Directory( request))" in region
assert "resourceCopy = XmStringCopy( FS_Pattern( request))" in region
assert "if (resourceCopy != NULL)" in region
assert "FS_Directory( new_w) = FS_Directory( current)" in region
assert "FS_Pattern( new_w) = FS_Pattern( current)" in region
assert "if (searchData.dir == NULL)" in region
assert "if (searchData.pattern == NULL)" in region
print("FileSelectionBox search resources preserve state on copy failure")
