#!/usr/bin/env python3
"""Ensure WSM config replies reject inconsistent array state."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
start = text.index("_WSMPackReply")
end = text.index("\n}\n", start)
function = text[start:end]
assert "reply->config_format.num_global_formats != 0" in function
assert "reply->config_format.global_formats == NULL" in function
assert "reply->config_format.window_formats == NULL" in function
assert "reply->config_format.icon_formats == NULL" in function
print("WSM config replies validate attribute arrays")
