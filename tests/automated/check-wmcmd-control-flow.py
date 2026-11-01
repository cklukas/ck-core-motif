#!/usr/bin/env python3
"""Check MWM gadget-info fallbacks have explicit control-flow boundaries."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c").read_text(
    encoding="utf-8"
)


def function(name: str) -> str:
    start = source.index(f"{name}(ClientData")
    return source[start:source.index("/*----------------------------------------------------------------------*", start)]


maximize = function("GetMaximizeInfo")
assert "if (maxFound == False)\n\t{\n\t  FillInvalidInfo (&maximize_button);\n\t}" in maximize

move = function("GetMoveInfo")
assert move.count("GadgetRectangle title = { INVALID, { INVALID, INVALID, INVALID, INVALID } };") == 1
assert move.count("GadgetRectangle menu = { INVALID, { INVALID, INVALID, INVALID, INVALID } };") == 1
assert "if (titleFound == False)\n\t {\n\t   FillInvalidInfo (&title);\n\t }" in move
assert "if (system_found == False)\n\t {\n\t   FillInvalidInfo (&menu);\n\t }" in move

resize = function("GetResizeInfo")
assert "if (!(pcd->decor & MWM_DECOR_RESIZEH))\n\t{\n\t  FillInvalidInfo (&gravity_resize);\n\t}" in resize
assert "for (i=0; i < filledCount; i++)\n\t{" in resize
assert "if (titleFound == False)\n\t  {\n\t    FillInvalidInfo (&title);\n\t  }" in resize

window_menu = function("GetWindowMenuPostInfo")
assert window_menu.count("GadgetRectangle title = { INVALID, { INVALID, INVALID, INVALID, INVALID } };") == 1
assert window_menu.count("GadgetRectangle menu = { INVALID, { INVALID, INVALID, INVALID, INVALID } };") == 1

print("PASS MWM gadget-info fallback control flow is explicit")
