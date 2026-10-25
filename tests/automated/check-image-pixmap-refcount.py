#!/usr/bin/env python3
"""Check XmDestroyPixmap cannot underflow a cached pixmap reference."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("XmDestroyPixmap(")
end = source.index("/****", start) if "/****" in source[start:] else len(source)
region = source[start:end]
assert "pix_entry->reference_count <= 0" in region
assert "_XmProcessUnlock();" in region
assert "_XmAppUnlock(app);" in region
assert "pix_entry->reference_count--;" in region
print("PASS XmDestroyPixmap preserves non-negative references")
