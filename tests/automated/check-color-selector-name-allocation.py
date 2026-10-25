#!/usr/bin/env python3
"""Guard ColorSelector color-name replacement against null strings."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c").read_text(
    encoding="utf-8"
)
start = text.index("static Boolean\nUpdateColorWindow(", 10000)
body = text[start:text.index("\n/*\tFunction Name:", start)]
if "char\t*new_name;" not in body:
    raise SystemExit("ColorSelector replacement buffer is missing")
if body.count("if (new_name == NULL)") < 3:
    raise SystemExit("ColorSelector name allocation checks are incomplete")
if "XtFree(XmColorS_color_name(csw));\n\t    XmColorS_color_name(csw) = new_name;" not in body:
    raise SystemExit("ColorSelector replacement does not swap after allocation")

print("ColorSelector name allocation checks passed")
