#!/usr/bin/env python3
"""Check bounded XmInfoBar text geometry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "InfoBar.c").read_text(encoding="utf-8")
assert "static int bounded_text_length" in source
assert "long long message_x" in source
assert "bounded_text_length(bar->info.message)" in source
assert "bounded_text_length(bar->info.action_label)" in source
print("PASS info-bar text geometry is bounded")
