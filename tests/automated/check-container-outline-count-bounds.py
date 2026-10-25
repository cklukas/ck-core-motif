#!/usr/bin/env python3
"""Check Container outline segment counting cannot overflow."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("LayoutOutlineDetail(\n\tWidget\twid)")
end = source.index("\n/************************************************************************", start + 1)
region = source[start:end]

assert "#include <limits.h>" in source
assert "n_outline_segs > INT_MAX - 2" in region
assert "outline_ready = False;" in region
assert "if (outline_ready && cw->container.outline_seg_count != n_outline_segs)" in region
assert region.index("n_outline_segs > INT_MAX - 2") < region.index("n_outline_segs += 2")
print("Container outline segment counts reject integer overflow")
