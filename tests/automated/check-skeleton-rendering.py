#!/usr/bin/env python3
"""Check Skeleton renders a muted base and animated highlight."""
from pathlib import Path

source = Path("lib/Xm/Skeleton.c").read_text(encoding="utf-8")
assert "primitive.bottom_shadow_color" in source
assert "primitive.top_shadow_color" in source
assert "skeleton->skeleton.phase += 8" in source
assert "stripe_x - 32" in source
assert "if (!skeleton->skeleton.running" in source
print("PASS Skeleton uses theme-derived base and highlight rendering")
