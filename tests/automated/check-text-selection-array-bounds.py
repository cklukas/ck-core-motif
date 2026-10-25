#!/usr/bin/env python3
"""Check TextIn selection-array copies handle bounds and allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text()
assert source.count("UINT_MAX / sizeof(XmTextScanType)") >= 2
assert source.count("(Cardinal)((size_t)data->sarraycount *") >= 2
assert "XmTextScanType *new_sarray" in source
assert "if (new_sarray == NULL)" in source
assert source.count("data->sarray = (XmTextScanType *)sarray;") >= 4
assert "memcpy((void *)new_sarray" in source
update_start = source.index("InputSetValues(")
update_end = source.index("static void \nInputDestroy", update_start)
update = source[update_start:update_end]
assert update.index("XtMalloc(") < update.index("memcpy((void *)new_sarray")
assert update.index("memcpy((void *)new_sarray") < update.rindex("XtFree((char *)temp_ptr)")
assert "data->sarray != (XmTextScanType *)sarray" in source
print("PASS TextIn selection-array copies handle allocation failure")
