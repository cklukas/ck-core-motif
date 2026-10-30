#!/usr/bin/env python3
"""Ensure ButtonBox initialization validates widget inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/ButtonBox.c").read_text()
start = source.index("static void Initialize(Widget req, Widget set,")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "req == NULL || set == NULL" in body
assert "!XtIsSubclass(set, xmButtonBoxWidgetClass)" in body
assert body.index("if (req == NULL") < body.index("req->core.width")
print("ButtonBox initialization validates widget inputs")
