#!/usr/bin/env python3
"""Ensure Badge QueryGeometry validates widget and reply inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Badge.c").read_text()
assert "static XtGeometryResult QueryGeometry" in source
assert "widget == NULL || reply == NULL" in source
assert "!XtIsSubclass(widget, xmBadgeWidgetClass)" in source
assert "return XtGeometryNo;" in source
print("Badge QueryGeometry validates widget and reply inputs")
