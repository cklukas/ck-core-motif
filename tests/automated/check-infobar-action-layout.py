#!/usr/bin/env python3
"""Check InfoBar action placement follows its measured label width."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "InfoBar.c").read_text(
    encoding="utf-8"
)
assert "static int ContentRight(XmInfoBarWidget bar)" in source
assert "static int ActionWidth(XmInfoBarWidget bar)" in source
assert "XmStringExtent(bar->info.font,label,&width,&height);" in source
assert "content_right-(int)action_width" in source
assert "message_right=MAX(content_left,action_left-8);" in source
assert "event->xbutton.x>=action_left&&event->xbutton.x<action_right" in source
assert "bar->core.width - 100" not in source
assert "bar->core.width>120" not in source
print("PASS InfoBar action label placement and hit testing use measured bounds")
