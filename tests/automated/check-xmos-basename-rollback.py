#!/usr/bin/env python3
"""Check Xmos basename conversion allocates before releasing its source."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("if (listWithFullPath)")
end = source.index("\n\t    }", start)
region = source[start:end]

assert "String          replacement;" in source
assert "replacement = XtMalloc(dirNameLen + 1);" in region
assert "if (replacement == NULL)" in region
assert "XtFree(entryPtr);\n\t\t      break;" in region
assert "strcpy(replacement, dirName);" in region
assert "XtFree(entryPtr);\n\t\t  entryPtr = replacement;" in region
assert region.index("replacement = XtMalloc") < region.index("XtFree(entryPtr)")
print("Xmos basename conversion preserves ownership on allocation failure")
