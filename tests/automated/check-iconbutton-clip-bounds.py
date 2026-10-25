#!/usr/bin/env python3
"""Check IconButton text clipping bounds use signed spacing arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconButton.c").read_text(encoding="utf-8")
assert "size = (int)XtWidth(iw) - (int)H_SPACE(iw);" in source
assert "size = (int)XtHeight(iw) - (int)V_SPACE(iw);" in source
assert "size = XtWidth(iw) - H_SPACE(iw);" not in source
assert "size = XtHeight(iw) - V_SPACE(iw);" not in source
print("icon-button text clipping bounds use signed arithmetic")
