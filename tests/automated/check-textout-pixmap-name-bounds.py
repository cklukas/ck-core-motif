#!/usr/bin/env python3
"""Check TextOut's fixed pixmap-name buffers use bounded formatting."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextOut.c").read_text()
assert 'snprintf(pixmap_name, sizeof(pixmap_name), "_XmText_%d_%d"' in source
assert 'snprintf(pixmap_name, sizeof(pixmap_name), "_XmText_AddMode_%d_%d"' in source
assert 'sprintf(pixmap_name' not in source
print("PASS TextOut pixmap names use bounded formatting")
