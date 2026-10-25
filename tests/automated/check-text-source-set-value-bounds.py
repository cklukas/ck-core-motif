#!/usr/bin/env python3
"""Check Text source set-value input bounds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("_XmStringSourceSetValue(")
end = source.index("\n\nBoolean \n_XmStringSourceHasSelection", start)
region = source[start:end]
assert "size_t value_length;" in region
assert "if (value == NULL)" in region
assert "value_length > (size_t)INT_MAX" in region
assert "block.length = (int)value_length;" in region
assert region.index("value_length > (size_t)INT_MAX") < region.index("(*source->SetSelection)")
print("PASS Text source set-value input is bounded")
