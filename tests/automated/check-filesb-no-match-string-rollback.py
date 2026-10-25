#!/usr/bin/env python3
"""Check FileSelectionBox no-match resource replacement is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
start = source.index("if(    FS_NoMatchString( new_w) !=")
end = source.index("\n    if(    !FS_QualifySearchDataProc", start)
region = source[start:end]
assert "noMatchCopy ;" in source
assert "noMatchCopy = XmStringCopy" in region
assert "if (noMatchCopy != NULL)" in region
assert "XmStringFree( FS_NoMatchString( current))" in region
assert "FS_NoMatchString( new_w) = FS_NoMatchString( current)" in region
print("FileSelectionBox no-match strings preserve the current value on failure")
