#!/usr/bin/env python3
"""Check MWM bounds the accelerator-to-menu-button walk."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmEvent.c").read_text(
    encoding="utf-8"
)
start = source.index("Boolean WmDispatchMenuEvent")
end = source.index("\n} /* END OF FUNCTION WmDispatchMenuEvent */", start)
region = source[start:end]
assert "menuBtnPtr = wmGD.menuActive->menuButtons;" in region
assert "menuButtonSlots = wmGD.menuActive->menuButtonSize;" in region
assert "while (keySpecs && menuBtnPtr != NULL && menuButtonSlots != 0)" in region
assert "--menuButtonSlots;" in region
assert "menuBtnPtr = &wmGD.menuActive->menuButtons[menuButtonSlots];" in region
assert "menuButtons + \n\t                 (wmGD.menuActive->menuButtonSize - 1)" not in region
assert "menuBtnPtr--;" not in region
print("PASS MWM menu accelerators stay within populated button-table slots")
