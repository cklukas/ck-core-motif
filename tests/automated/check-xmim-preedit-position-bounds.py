#!/usr/bin/env python3
"""Check XIM preedit-position geometry handles narrow focused widgets."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()

assert "int available_width, available_height;" in source
assert "available_width = (int) XtWidth(im_info->current_widget) -" in source
assert "available_height = (int) XtHeight(im_info->current_widget) -" in source
assert "rect_preedit.width = MIN(icp->preedit_width, available_width);" in source
assert "rect_preedit.height = MIN(icp->sp_height, available_height);" in source

print("Xm preedit-position geometry is bounded")
