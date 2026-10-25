#!/usr/bin/env python3
"""Check that RowColumn menu resource getters handle allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowColumn.c").read_text(encoding="utf-8")
accelerator = source.index("GetMenuAccelerator(")
accelerator_end = source.index("/*\n * Copy the String in XmNmenuPost", accelerator)
accelerator_region = source[accelerator:accelerator_end]
assert "if (data != NULL)" in accelerator_region
assert "*value = (XtArgVal) data" in accelerator_region
post = source.index("GetMenuPost(")
post_end = source.index("/*\n * Copy the XmString", post)
post_region = source[post:post_end]
assert "String copy = XtNewString(RC_MenuPost(rc));" in post_region
assert "*value = (XtArgVal) copy" in post_region
print("PASS RowColumn menu resource allocations are guarded")
