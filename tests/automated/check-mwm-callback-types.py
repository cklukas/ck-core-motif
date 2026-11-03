#!/usr/bin/env python3
"""Verify MWM callbacks use the Xt void-return callback ABI."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(
    encoding="utf-8"
)

assert "static void\nAdjustTearOffControlCallback" in source
assert "(void)AdjustTearOffControl(cascade, closure, cbackdata);" in source
assert "(XtCallbackProc)AdjustTearOffControl" not in source
assert "XmNcascadingCallback,\n\t\t\t\t      AdjustTearOffControlCallback," in source

print("PASS MWM tear-off callback uses the Xt callback ABI")
