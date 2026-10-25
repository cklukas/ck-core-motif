#!/usr/bin/env python3
"""Check scaled-pixmap construction rejects absent color state safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("_XmGetScaledPixmap(")
start = source.index("Pixmap", start - 80)
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "image_name == NULL || acc_color == NULL" in function
guard = function.index("image_name == NULL || acc_color == NULL")
assert guard < function.index("memcpy((void*)pix_entry->acc_color")
assert guard < function.index("acc_color->foreground")
print("PASS scaled-pixmap color input guard")
