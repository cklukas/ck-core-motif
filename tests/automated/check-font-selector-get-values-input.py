#!/usr/bin/env python3
"""Ensure FontSelector GetValuesHook validates its inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/FontS.c").read_text()
start = source.index("GetValuesHook(Widget w, ArgList args, Cardinal * num_args)")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "w == NULL || args == NULL || num_args == NULL" in body
assert "!XtIsSubclass(w, xmFontSelectorWidgetClass)" in body
assert body.index("if (w == NULL") < body.index("for (i = 0")
print("FontSelector GetValuesHook validates its inputs")
