#!/usr/bin/env python3
"""Check ColorSelector channels and filename comparisons remain bounded."""

from pathlib import Path


source = Path("lib/Xm/ColorS.c").read_text(encoding="utf-8")

helper_start = source.index("ColorSelectorChannelValue(int value)")
helper_end = source.index("static Boolean\nParseColorChannels", helper_start)
helper = source[helper_start:helper_end]
assert "if (value < 0)" in helper
assert "if (value > 255)" in helper

diff_start = source.index("static Boolean AreDiff")
diff_end = source.index("/*      Function Name: SetValues", diff_start)
diff = source[diff_start:diff_end]
assert "return strcmp(s1, s2) != 0;" in diff

callback_start = source.index("slider_changed(Widget w")
callback_end = source.index("static Boolean\nUpdateColorWindow", callback_start)
callback = source[callback_start:callback_end]
assert callback.count("ColorSelectorChannelValue(scale->value)") == 3

update_start = source.index("static Boolean\nUpdateColorWindow")
update_end = source.index("static void\nlist_selected", update_start)
update = source[update_start:update_end]
assert "red = ColorSelectorChannelValue(XmColorS_slider_red(csw));" in update
assert "green = ColorSelectorChannelValue(XmColorS_slider_green(csw));" in update
assert "blue = ColorSelectorChannelValue(XmColorS_slider_blue(csw));" in update
assert "color.red = (unsigned short)(red * 256);" in update
assert "color.green = (unsigned short)(green * 256);" in update
assert "color.blue = (unsigned short)(blue * 256);" in update

print("ColorSelector channel values and filename comparisons are bounded")
