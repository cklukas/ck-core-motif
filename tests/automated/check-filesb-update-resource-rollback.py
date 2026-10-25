#!/usr/bin/env python3
"""Check FileSelectionBoxUpdate resource copies are failure-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
start = source.index("static void \nFileSelectionBoxUpdate(")
end = source.index("\n/****************************************************************/", start + 1)
region = source[start:end]

assert "XmString        resourceCopy" in region
assert "resourceCopy = qualifiedSearchData.dir != NULL" in region
assert "resourceCopy = qualifiedSearchData.pattern != NULL" in region
assert "qualifiedSearchData.dir == NULL || resourceCopy != NULL" in region
assert "qualifiedSearchData.pattern == NULL || resourceCopy != NULL" in region
assert "FS_Directory( fs) = resourceCopy" in region
assert "FS_Pattern( fs) = resourceCopy" in region
assert region.count("XmStringFree( FS_Directory( fs))") == 1
assert region.count("XmStringFree( FS_Pattern( fs))") == 1
print("FileSelectionBoxUpdate preserves resources on copy failure")
