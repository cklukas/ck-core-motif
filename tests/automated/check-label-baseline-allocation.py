#!/usr/bin/env python3
"""Check Label baseline cache and output allocation failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Label.c").read_text()
start = source.index("static Boolean\nXmLabelGetBaselines(")
end = source.index("\n/************************************************************************\n *", start)
region = source[start:end]

assert "Dimension *new_baselines;" in region
assert "if (lw->label.baselines == NULL" in region
assert "new_baselines = (Dimension *)XtRealloc" in region
assert "if (new_baselines == NULL)" in region
assert "if (*baselines == NULL)" in region
assert "if (count == 0)" in region
assert "(size_t)count >= SIZE_MAX / sizeof(*new_baselines)" in region
assert "((size_t)count + 1) * sizeof(*new_baselines)" in region

print("PASS Label baseline allocation checks")
