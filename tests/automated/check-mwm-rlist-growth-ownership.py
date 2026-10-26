#!/usr/bin/env python3
"""Check that MWM rectangle-list growth publishes and validates its buffer."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmGraphics.c").read_text(
    encoding="utf-8"
)
start = source.index("Boolean ExtendRList")
end = source.index("\n}", start) + 2
region = source[start:end]

assert "prl->used > prl->allocated" in region
assert "prl->prect == NULL" in region
assert "copy_bytes = (size_t)prl->used * sizeof(XRectangle);" in region
assert "prl->prect = pNewRect;" in region
assert "prl->allocated = (int)total;" in region
print("PASS MWM rectangle-list growth preserves ownership and validates state")
