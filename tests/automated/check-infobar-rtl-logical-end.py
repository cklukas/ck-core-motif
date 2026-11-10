#!/usr/bin/env python3
"""Guard InfoBar's shared logical-end rendering and hit-test geometry."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "InfoBar.c").read_text(
    encoding="utf-8"
)

assert "static Boolean IsRtl(XmInfoBarWidget bar)" in source
assert "static int CloseLeft(XmInfoBarWidget bar){return IsRtl(bar)?4:" in source
assert "ContentLeft(bar)" in source
assert "action_left=content_left;action_right=content_right;" in source
assert "int action_left=ContentLeft(bar),action_right=ContentRight(bar);" in source
assert "if(IsRtl(bar))action_right=MIN(action_right,action_left+width);" in source
assert "int close_x=bar->core.width>=24?CloseLeft(bar):0;" in source
print("InfoBar mirrors logical-end action and close geometry in RTL")
