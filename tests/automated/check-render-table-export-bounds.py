#!/usr/bin/env python3
"""Check render-table property export capacity and formatting contracts."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(encoding="utf-8")
start = text.index("#define CVTaddString")
end = text.index("typedef enum", start)
region = text[start:end]
assert "while (new_size < required)" in region
assert "memcpy(buffer + chars_used, src" in region
assert "XmRenderFormat(temp, sizeof(temp)" in region
assert "strcat(temp, \" ], \")" not in region
assert 'CVTaddString(buffer, "\\n", 1);' in region
assert 'CVTaddString(buffer, "\\n", size);' not in region
assert "chars_used >= UINT_MAX" in region
print("render-table property export is bounded")
