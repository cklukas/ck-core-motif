#!/usr/bin/env python3
"""Check idempotent XIM reference removal."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text(encoding="utf-8")
start = text.index("remove_ref(XmImRefInfo")
end = text.index("/* Convert a VaArgList", start)
region = text[start:end]
assert "refs == NULL || refs->num_refs == 0" in region
assert "Cardinal index;" in region
assert "for (index = 0; index < refs->num_refs; index++)" in region
assert "if (index == refs->num_refs)" in region
assert "refs->num_refs--;" in region
assert region.index("if (index == refs->num_refs)") < region.index("refs->num_refs--;")
assert "refs->num_refs <= (refs->max_refs - 1) / 3" in region
assert "refs->num_refs * 3" not in region
print("XIM reference removal is idempotent")
