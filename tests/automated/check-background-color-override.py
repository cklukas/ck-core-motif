#!/usr/bin/env python3
"""Check that the legacy background color override is shared by get/set."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "Obso2_0.c").read_text(encoding="utf-8")
assert "static String default_background_color_spec_override = NULL;" in text
getter = text[text.index("_XmGetDefaultBackgroundColorSpec") : text.index("_XmSetDefaultBackgroundColorSpec")]
setter = text[text.index("_XmSetDefaultBackgroundColorSpec") : text.index("/*\n * GLOBAL VARIABLES", text.index("_XmSetDefaultBackgroundColorSpec"))]
assert "default_background_color_spec_override != NULL" in getter
assert "default_background_color_spec_override = XtNewString(new_color_spec);" in setter
assert "static Boolean app_defined" not in setter
assert "strcpy(default_background_color_spec" not in setter
print("background color override is shared and owned")
