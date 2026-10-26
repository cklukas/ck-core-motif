#!/usr/bin/env python3
"""Check TabStack coordinate lookup rejects a null widget handle."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabStack.c").read_text(encoding="utf-8")
start = source.index("XmTabStackXYToWidget")
region = source[start:]
assert "if (widget == NULL)" in region
assert region.index("if (widget == NULL)") < region.index("_XmWidgetToAppContext(widget)")
print("PASS TabStack coordinate lookup validates null widgets")
