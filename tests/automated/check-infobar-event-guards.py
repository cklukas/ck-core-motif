#!/usr/bin/env python3
"""Ensure InfoBar event dispatch validates widget and event inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "InfoBar.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Events(Widget widget")
body = source[start:source.index("\nstatic Boolean SetValues", start)]
assert "bar==NULL||event==NULL||!XmIsInfoBar(widget)" in body
print("InfoBar event callbacks validate widget and event inputs")
