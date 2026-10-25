#!/usr/bin/env python3
"""Check that Font Selector default pattern formatting is bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nSetDisplayedFont")
region = source[start:source.index("static void\n", start + 20)]
assert 'snprintf(buf, sizeof(buf), "--%s-%s-%s----0-%d-%d-*--%s"' in region
assert 'sprintf(buf, "--%s-%s-%s----0-%d-%d-*--%s"' not in region
print("Font Selector default pattern formatting is bounded")
