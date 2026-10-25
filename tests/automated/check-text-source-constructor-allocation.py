#!/usr/bin/env python3
"""Check transactional allocation handling in the Text source constructor."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("_XmStringSourceCreate(")
end = source.index("\nvoid \n_XmStringSourceDestroy", start)
region = source[start:end]
assert "if (source == NULL)" in region
assert "if (data == NULL)" in region
assert "if (value == NULL)" in region
assert region.count("goto allocation_failure;") >= 4
assert "allocation_failure:" in region
assert "XtFree((char *)data->ptr);" in region
assert "return NULL;" in region
print("PASS Text source construction is transactional")
