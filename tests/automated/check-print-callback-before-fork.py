#!/usr/bin/env python3
"""Check XmPrintToFile prepares callback state before fork."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PrintS.c").read_text()
start = source.index("XmPrintToFile(")
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert region.index("XtMalloc(sizeof(FileCallbackRec))") < region.index("pid = fork()")
assert "XtFree((char *)callback);" in region
assert "callback->finish_proc = finish_proc;" in region
print("PrintShell callback state is prepared before fork")
