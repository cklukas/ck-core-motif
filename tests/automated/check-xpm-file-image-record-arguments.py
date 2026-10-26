#!/usr/bin/env python3
"""Ensure the raw XPM-image reader validates all required output records."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmRdFToI.c").read_text()
start = source.index("XpmReadFileToXpmImage(\n")
body = source[start:source.index("#endif /* CXPMPROG */", start)]
guard = body.index("if (filename == NULL")
before_init = body[guard:body.index("xpmInitXpmImage", guard)]
assert "image == NULL" in before_init
assert "info == NULL" in before_init
assert "return XpmFileInvalid;" in before_init
print("PASS XpmReadFileToXpmImage validates output records")
