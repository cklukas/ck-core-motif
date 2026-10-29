#!/usr/bin/env python3
"""Check legacy ToolTip child and timer failure paths are null-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToolTip.c").read_text()
data_start = source.index("ToolTipGetData (Widget w)")
data_end = source.index("static void\nToolTipUnpost", data_start)
data = source[data_start:data_end]
unpost_start = source.index("ToolTipUnpost", data_end)
unpost_end = source.index("static void\nToolTipPostFinish", unpost_start)
unpost = source[unpost_start:unpost_end]
finish_start = source.index("ToolTipPostFinish", unpost_end)
finish_end = source.index("static void\nToolTipPost", finish_start)
finish = source[finish_start:finish_end]
post_start = source.index("ToolTipPost (XtPointer", unpost_end)
post_end = source.index("/*\n=====================================", post_start)
post = source[post_start:post_end]
assert "if (shell == NULL)" in data
assert "if (ttp->label == NULL)" in data
assert "Widget shell = XtParent (ttp->label);" in unpost
assert "if (ttp->label == NULL)" in finish
assert "if (ttp->slider == NULL)" in post
assert "XtAddCallback (ttp->slider" in post
print("PASS ToolTip child and timer failure paths are null-safe")
