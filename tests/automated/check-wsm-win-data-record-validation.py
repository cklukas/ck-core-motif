#!/usr/bin/env python3
"""Check that WSM packing rejects malformed window-data records."""

from pathlib import Path


source = (
    Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c"
).read_text(encoding="utf-8")
start = source.rindex("WinDataSizePacked(Display")
end = source.index("/************************************************************", start)
function = source[start:end]

assert "(num != 0 && win_data == NULL)" in function
assert "attr->size != 8 && attr->size != 16 && attr->size != 32" in function
assert "case WSM_VALUE_DATA:" in function
assert "if (attr->is_list)" in function
assert "case WSM_CHAR_LIST_DATA:" in function
assert "attr->size != 8" in function
assert "win_data->data.char_ptr == NULL" in function
assert "case WSM_SHORT_LIST_DATA:" in function
assert "attr->size != 16" in function
assert "win_data->data.short_ptr == NULL" in function
assert "case WSM_LONG_LIST_DATA:" in function
assert "attr->size != 32" in function
assert "win_data->data.long_ptr == NULL" in function
assert "default:\n\t    return(-1);" in function

print("WSM packing validates window-data type, width, and payload contracts")
