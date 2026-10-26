#!/usr/bin/env python3
"""Check MWM default-color cache growth is bounded and transactional."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text(
    encoding="utf-8"
)
start = source.index("XmColorData * _WmGetDefaultColors")
end = source.index("} /* END OF FUNCTION _WmGetDefaultColors */", start)
region = source[start:end]
assert "size > INT_MAX - 10" in region
assert "UINT_MAX / sizeof(XmColorData)" in region
assert "XmColorData *new_set;" in region
assert "new_set = (XmColorData *)XtRealloc" in region
assert "if (new_set == NULL)" in region
assert "defaultSet[setId] = new_set" in region
assert "WmRealloc" not in region
print("PASS MWM default-color cache growth is bounded and transactional")
