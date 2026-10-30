#!/usr/bin/env python3
"""Ensure FontSelector GetValuesHook skips nameless requests."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/FontS.c").read_text()
start = source.index("GetValuesHook(Widget w, ArgList args, Cardinal * num_args)")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "if (args[i].name == NULL || args[i].value == NULL)" in body
assert body.index("if (args[i].name == NULL") < body.index("if (streq(args[i].name")
print("FontSelector GetValuesHook skips nameless requests")
