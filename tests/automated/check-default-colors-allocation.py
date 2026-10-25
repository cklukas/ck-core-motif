#!/usr/bin/env python3
"""Check that legacy default-color cache growth is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Obso2_0.c").read_text(encoding="utf-8")
start = source.index("XmColorData * \n_XmGetDefaultColors(")
end = source.index("static int \n_XmBrightness", start)
region = source[start:end]

assert "new_default_set_size" in region
assert "SIZE_MAX / sizeof(XmColorData *)" in region
assert "new_default_set = (XmColorData **) XtRealloc" in region
assert "if (new_default_set == NULL)" in region
assert "default_set = new_default_set;" in region
print("PASS legacy default-color cache growth is guarded")
