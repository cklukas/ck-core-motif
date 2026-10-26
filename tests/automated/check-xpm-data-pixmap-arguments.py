#!/usr/bin/env python3
"""Ensure XpmCreateDataFromPixmap validates display and source handles."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrP.c").read_text()
start = source.index("XpmCreateDataFromPixmap(\n")
body = source[start:]
guard = body.index("if (display == NULL")
before_geometry = body[guard:body.index("/* get geometry */", guard)]
assert "data_return == NULL" in before_geometry
assert "pixmap == 0 && shapemask == 0" in before_geometry
assert "return XpmFileInvalid;" in before_geometry
print("PASS XpmCreateDataFromPixmap validates public arguments")
