#!/usr/bin/env python3
"""Check that MWM validates its screen-record allocation count."""

from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmInitWs.c").read_text(
    encoding="utf-8"
)
start = source.index("ProcessGlobalScreenResources ();", source.index("void InitWm"))
end = source.index("#ifdef WSM", start)
validation = source[start:end]
allocation = source[source.index("Allocate data and initialize for screens we manage", start):]

assert "wmGD.numScreens <= 0" in validation
assert "SIZE_MAX / sizeof(WmScreenData)" in validation
assert "UINT_MAX / sizeof(WmScreenData)" in validation
assert "ExitWM (WM_ERROR_EXIT_VALUE);" in validation
assert "XtCalloc ((Cardinal)wmGD.numScreens, sizeof(WmScreenData))" in allocation

print("MWM validates screen counts before allocating screen records")
