#!/usr/bin/env python3
"""Check MWM system-modal screen geometry before unsigned X11 conversion."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmWinList.c").read_text(
    encoding="utf-8"
)
start = source.index("void SetupSystemModalState")
end = source.index("} /* END OF FUNCTION SetupSystemModalState", start)
region = source[start:end]
assert "int                  screen_width;" in region
assert "int                  screen_height;" in region
assert "screen_width = DisplayWidth (DISPLAY, pSD->screen);" in region
assert "screen_height = DisplayHeight (DISPLAY, pSD->screen);" in region
assert "screen_width <= 0 || screen_height <= 0" in region
assert "(unsigned int)screen_width" in region
assert "(unsigned int)screen_height" in region
print("PASS MWM system-modal screen dimensions are validated before conversion")
