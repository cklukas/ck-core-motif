#!/usr/bin/env python3
"""Ensure receiver metadata does not truncate the wire site count."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
start = source.index("void \n_XmSetDragReceiverInfo(")
body = source[start:]
assert "numDropSites > UINT16_MAX" in body
assert "infoRecPtr->num_drop_sites = numDropSites;" in body
print("PASS drag receiver site counts are bounded to CARD16")
