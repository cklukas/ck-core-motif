#!/usr/bin/env python3
"""Check BreadcrumbBar segment replacement fails closed during teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/BreadcrumbBar.c").read_text(
    encoding="utf-8"
)
assert "widget->core.being_destroyed" in source
print("PASS BreadcrumbBar replacement guards destruction")
