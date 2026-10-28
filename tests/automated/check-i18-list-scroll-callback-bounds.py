#!/usr/bin/env python3
"""Check I18List scrollbar callback values cannot wrap short."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "static short bounded_short(long long value)" in source
assert "VScroll((Widget) ilist, bounded_short((long long)scroll_info->value -" in source
assert "HScroll((Widget)ilist, bounded_short((long long)scroll_info->value));" in source
assert "(short) (scroll_info->value -" not in source
assert "(short) scroll_info->value" not in source
print("PASS I18List scrollbar callback values stay bounded")
