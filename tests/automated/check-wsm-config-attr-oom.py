#!/usr/bin/env python3
"""Check WSM config attribute allocation failure rolls back partial state."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/pack.c"
text = source.read_text(encoding="utf-8")
start = text.index("case WSM_CONFIG_FMT:", text.index("_WSMUnpackReply"))
end = text.index("\n\tbreak;", start)
region = text[start:end]

assert "if (fmt->attr_list == NULL)" in region
assert "fmt->num_attrs = 0;" in region
assert "_WSMClearConfigScreenInfo(dpy, screen_num);" in region
assert region.index("if (fmt->attr_list == NULL)") < region.index("for (i = 0; i < fmt->num_attrs")
print("WSM config attribute allocation rolls back safely")
