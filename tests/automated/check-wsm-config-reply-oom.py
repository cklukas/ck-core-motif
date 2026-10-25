#!/usr/bin/env python3
"""Check WSM config-format replies tolerate missing screen cache state."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/pack.c"
text = source.read_text(encoding="utf-8")
start = text.index("case WSM_CONFIG_FMT:", text.index("_WSMUnpackReply"))
end = text.index("\n\tbreak;", start)
region = text[start:end]

assert "WSMScreenInfo *scr_info = _WSMGetScreenInfo(dpy, screen_num);" in region
assert "if (scr_info == NULL)" in region
assert "return;" in region
assert region.index("if (scr_info == NULL)") < region.index("config_format->accepts_diffs")
print("WSM config-format replies handle missing screen state")
