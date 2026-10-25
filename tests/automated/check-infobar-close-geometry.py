#!/usr/bin/env python3
"""Check InfoBar close-icon drawing is bounded for small widgets."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "InfoBar.c").read_text(encoding="utf-8")
start = source.index("if (bar->info.closable &&")
end = source.index("\n    XFreeFont", start)
region = source[start:end]
assert "bar->core.width >= 18" in region
assert "bar->core.height >= 8" in region
assert "(int)bar->core.width - 18" in region
print("info-bar close-icon geometry is bounded")
