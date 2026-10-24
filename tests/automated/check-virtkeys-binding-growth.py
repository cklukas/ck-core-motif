#!/usr/bin/env python3
"""Check size-safe virtual-key binding file growth."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text(encoding="utf-8")
start = text.index("_XmVirtKeysLoadFileBindings")
end = text.index("static void", start)
region = text[start:end]
assert "size_t buffersize" in region
assert "size_t count" in region
assert "required = count + line_length + 1" in region
assert "required > (size_t)UINT_MAX" in region
assert "while (new_size < required)" in region
assert "complete = False" in region
assert "if (!complete)" in region
print("virtual-key binding growth is size-safe")
