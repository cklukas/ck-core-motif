#!/usr/bin/env python3
"""Check InfoBar action placement follows its measured label width."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "InfoBar.c").read_text(
    encoding="utf-8"
)
assert "static int ContentRight(XmInfoBarWidget bar)" in source
assert "static int ActionLeft(XmInfoBarWidget bar)" in source
assert "XmStringExtent(bar->info.font,label,&width,&height);" in source
assert "right-(int)width" in source
assert "message_right = action_string ? action_x - 8 : content_right;" in source
assert "event->xbutton.x>=action_x&&event->xbutton.x<ContentRight(bar)" in source
assert "bar->core.width - 100" not in source
assert "bar->core.width>120" not in source
print("PASS InfoBar action label placement and hit testing use measured bounds")
