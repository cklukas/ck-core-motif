#!/usr/bin/env python3
"""Check ColorSelector preferred dimensions saturate safely."""

from pathlib import Path


source = Path("lib/Xm/ColorS.c").read_text(encoding="utf-8")

helper_start = source.index("ColorSelectorAddDimension(uint64_t total")
helper_end = source.index("static Boolean\nParseColorChannels", helper_start)
helper = source[helper_start:helper_end]
assert "amount > (uint64_t)USHRT_MAX - total" in helper
assert "return (uint64_t)USHRT_MAX;" in helper

preferred_start = source.index("CalcPreferredSize(XmColorSelectorWidget csw")
preferred_end = source.index("color_name_changed(XmColorSelectorWidget", preferred_start)
preferred = source[preferred_start:preferred_end]
assert "uint64_t preferred_width = 0, preferred_height = 0;" in preferred
assert "child_width = (uint64_t)geo.width + 2 * (uint64_t)geo.border_width;" in preferred
assert "child_height = (uint64_t)geo.height + 2 * (uint64_t)geo.border_width;" in preferred
assert "ColorSelectorAddDimension(preferred_height" in preferred
assert "*width = ColorSelectorDimension((long long)preferred_width);" in preferred
assert "*height = ColorSelectorDimension((long long)preferred_height);" in preferred
assert "*height +=" not in preferred
assert "*width +=" not in preferred

print("ColorSelector preferred dimensions saturate instead of wrapping")
