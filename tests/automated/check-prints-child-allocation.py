#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/PrintS.c").read_text()
start = source.index("static void ChildPrintToFile")
end = source.index("XtEnum XmPrintToFile", start)
region = source[start:end]
assert "file_desc = (FileDescRec *) XtMalloc(sizeof(FileDescRec));" in region
assert "if (file_desc == NULL)" in region
assert "file_desc->file_name = XtNewString(file_name);" in region
assert "if (file_desc->file_name == NULL)" in region
assert "XtFree((char *)file_desc);" in region
assert "_exit(1);" in region
print("PrintS child descriptor allocation guards present")
