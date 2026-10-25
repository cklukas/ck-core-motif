#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/List.c").read_text()
assert source.count("SIZE_MAX / sizeof(ElementPtr)") >= 2
assert source.count("SIZE_MAX / sizeof(XmString)") >= 3
assert source.count("ElementPtr *new_internal") >= 2
assert source.count("XmString *new_items") >= 2
assert source.count("if (new_items != NULL)") >= 2
print("XmList deletion allocation guards present")
