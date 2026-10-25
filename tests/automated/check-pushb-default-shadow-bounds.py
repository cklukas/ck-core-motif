#!/usr/bin/env python3
"""Check PushButton default-shadow erasure handles collapsed geometry."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PushB.c").read_text()

assert "if (adjust < 0)" in source
assert "box.x > adjust &&" in source
assert "(int)pb->core.width > 2 * adjust" in source
assert "(int)pb->core.height > 2 * adjust" in source

print("PushButton default-shadow erasure geometry is bounded")
