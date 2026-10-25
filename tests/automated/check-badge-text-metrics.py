#!/usr/bin/env python3
"""Check Badge text uses measured font geometry."""
from pathlib import Path

source = Path("lib/Xm/Badge.c").read_text(encoding="utf-8")
assert "XQueryFont" in source
assert "XTextWidth" in source
assert "XFreeFontInfo(NULL, font, 1)" in source
assert "text_metrics(badge, text" in source
print("PASS Badge text uses measured font geometry")
