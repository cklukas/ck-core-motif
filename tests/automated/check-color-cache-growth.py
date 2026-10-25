#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Color.c").read_text()
assert "#include <stdint.h>" in source
assert "SIZE_MAX / sizeof(XmColorData)" in source
assert "UINT_MAX / sizeof(XmColorData)" in source
assert source.count("XmColorData *new_set") >= 3
assert source.count("if (new_set == NULL)") >= 3
assert "default_set = new_set;" in source
assert "Color_Set = new_set;" in source
assert "default_set_size += 10" not in source
assert "Set_Size += 10" not in source
print("Color cache growth guards present")
