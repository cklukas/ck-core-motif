#!/usr/bin/env python3
"""Ensure Badge Redisplay validates its widget."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Badge.c").read_text()
assert "static void Redisplay(Widget widget" in source
assert "widget == NULL || !XtIsSubclass(widget, xmBadgeWidgetClass)" in source
print("Badge Redisplay validates its widget")
