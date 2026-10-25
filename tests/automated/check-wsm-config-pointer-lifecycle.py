#!/usr/bin/env python3
"""Ensure WSM config pointers are initialized and freed independently of counts."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "disp.c").read_text(
    encoding="utf-8"
)
start = text.index("_WSMGetScreenInfo")
end = text.index("\n}\n", start)
init = text[start:end]
for name in ("global", "window", "icon"):
    assert f"screen_info->{name}.attr_list = NULL;" in init
start = text.index("_WSMClearConfigScreenInfo")
end = text.index("\n}\n", start)
clear = text[start:end]
for name in ("global", "window", "icon"):
    assert f"if (screen_info->{name}.attr_list != NULL)" in clear
print("WSM config pointers have explicit lifetime")
