#!/usr/bin/env python3
"""Check accessibility actions reject widgets entering Xt teardown."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
source = (ROOT / "lib/Xm/Accessible.c").read_text(encoding="utf-8")
assert "n->widget && n->widget->core.being_destroyed" in source
print("PASS accessibility actions guard widget destruction")
