#!/usr/bin/env python3
"""Guard ColorSelector color-name replacement against null strings."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c").read_text(
    encoding="utf-8"
)
start = text.index("static Boolean\nUpdateColorWindow(", 10000)
body = text[start:text.index("\n/*\tFunction Name:", start)]
if "char *new_name;" not in body:
    raise SystemExit("ColorSelector replacement buffer is missing")
if body.count("if (new_name == NULL)") < 3:
    raise SystemExit("ColorSelector name allocation checks are incomplete")
if "XtFree(XmColorS_color_name(csw));\n\t    XmColorS_color_name(csw) = new_name;" not in body:
    raise SystemExit("ColorSelector replacement does not swap after allocation")

print("ColorSelector name allocation checks passed")

selected_start = text.rindex("list_selected(Widget")
selected = text[selected_start:text.index("/*      Function Name:", selected_start)]
if "String replacement;" not in selected:
    raise SystemExit("ColorSelector list selection buffer is missing")
if "if (replacement == NULL)" not in selected:
    raise SystemExit("ColorSelector list selection does not handle unparse failure")
if "color_name_changed(csw, replacement)" not in selected:
    raise SystemExit("ColorSelector list selection is not transactional")
