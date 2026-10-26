#!/usr/bin/env python3
"""Check Badge count API rejects null widget handles before casting."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Badge.c").read_text(encoding="utf-8")
start = source.index("int XmBadgeGetDisplayedCount")
region = source[start:]
assert "if (widget == NULL || !XmIsBadge(widget)) return -1;" in region
assert region.index("if (widget == NULL") < region.index("badge = (XmBadgeWidget)widget;")
print("PASS Badge count API validates null widgets before casting")
