#!/usr/bin/env python3
"""Check that Text source block-buffer growth preserves allocation failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("static XmTextPosition \nReadSource(")
end = source.index("\nvoid\n_XmTextValidate", start)
region = source[start:end]

assert "char *new_value;" in region
assert "new_value = (char *)XtMalloc(block_bytes);" in region
assert "new_value = (char *)XtRealloc(data->value, block_bytes);" in region
assert region.count("if (new_value == NULL)") == 2
assert region.count("return 0;") >= 4
assert "data->value = new_value;" in region
assert "if (data->value == NULL)" in region
print("PASS Text source block-buffer growth checks allocation failures")
