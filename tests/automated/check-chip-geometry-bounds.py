#!/usr/bin/env python3
"""Check bounded XmChip geometry calculations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Chip.c").read_text(encoding="utf-8")
start = source.index("static void label_metrics")
end = source.index("static void Redisplay", start)
region = source[start:end]

assert "text_length > (size_t)USHRT_MAX / 8" in region
assert "total_width > USHRT_MAX" in region
assert "*height > USHRT_MAX - 10" in region
print("PASS chip geometry is bounded")
