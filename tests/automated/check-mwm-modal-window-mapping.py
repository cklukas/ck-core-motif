#!/usr/bin/env python3
"""Check realized-window validation before MWM modal mapping."""

from pathlib import Path


source = Path("clients/mwm/WmWinList.c").read_text(encoding="utf-8")
start = source.index("void SetupSystemModalState")
end = source.index("END OF FUNCTION SetupSystemModalState", start)
body = source[start:end]

assert "pSD->inputScreenWindow != None" in body
map_pos = body.index("XMapRaised")
guard_pos = body.index("inputScreenWindow != None")
assert guard_pos < map_pos

print("PASS MWM modal mapping validates the window resource")
