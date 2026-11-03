#!/usr/bin/env python3
"""Verify MWM menu-button compaction keeps its unsigned count domain."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean ModifyClientCommandForMenuSpec")
end = source.index("static void ModifyClientCommandID", start)
body = source[start:end]

assert "unsigned int i, freebutton, buttoncount;" in body
assert "\n    int i, freebutton, buttoncount;" not in body
assert "buttoncount = menuSpec->menuButtonCount;" in body
assert "for (i = 0; i < menuSpec->menuButtonCount; ++i)" in body
assert "for (i = 0; i < buttoncount; ++i)" in body

print("PASS MWM client-command menu counts preserve unsigned bounds")
