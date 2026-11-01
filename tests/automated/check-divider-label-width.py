#!/usr/bin/env python3
"""Keep Divider dimensions saturated after measured size arithmetic."""
from pathlib import Path
text = (Path(__file__).parents[2] / "lib/Xm/Divider.c").read_text()
assert "divider->divider.label && divider->divider.font" in text
assert "unsigned long w, h;" in text
assert "*width = (Dimension)MIN(w, USHRT_MAX);" in text
assert "*height = (Dimension)MIN(h, USHRT_MAX);" in text
assert "natural_size(divider, &width, &height)" in text
assert "natural_size(divider, &reply->width, &reply->height)" in text
print("PASS Divider measured dimensions are saturated")
