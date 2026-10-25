#!/usr/bin/env python3
"""Check PushButton emphasis fallbacks do not assert on invalid resources."""

from pathlib import Path


for name in ("lib/Xm/PushB.c", "lib/Xm/PushBG.c"):
    source = Path(name).read_text()
    assert "default_button_emphasis" in source
    assert "assert(FALSE);" not in source

print("PushButton emphasis invalid values fall back safely")
