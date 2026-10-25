#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Draw.c").read_text()
assert "#include <stdint.h>" in source
assert "size_t segment_count" in source
assert "SIZE_MAX / sizeof(XSegment)" in source
assert "XSegment *new_segms" in source
assert "if (new_segms == NULL)" in source
assert "segms = new_segms;" in source
assert "XtRealloc" not in source
print("Draw shadow segment-buffer growth guards present")
