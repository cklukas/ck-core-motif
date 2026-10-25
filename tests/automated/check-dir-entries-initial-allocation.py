#!/usr/bin/env python3
"""Check that directory-entry list initialization handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text(encoding="utf-8")
start = source.index("_XmOSGetDirEntries(String")
end = source.index("/****************************************************************\n * _XmOSBuildFileList", start)
region = source[start:end]
assert "*pEntries = (String *) XtMalloc(FILE_LIST_BLOCK * sizeof(char *));" in region
assert "if (*pEntries == NULL)" in region
assert "*pNumAlloc = 0;" in region
assert "_XmProcessUnlock();" in region
print("PASS directory-entry initial allocation is guarded")
