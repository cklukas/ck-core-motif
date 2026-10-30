#!/usr/bin/env python3
"""Ensure Avatar QueryGeometry validates widget and reply inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Avatar.c").read_text()
start = source.rindex("static XtGeometryResult QueryGeometry")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "widget == NULL || reply == NULL" in body
assert "!XtIsSubclass(widget, xmAvatarWidgetClass)" in body
assert "return XtGeometryNo;" in body
assert body.index("if (widget == NULL") < body.index("reply->request_mode")
print("Avatar QueryGeometry validates widget and reply inputs")
