#!/usr/bin/env python3
"""Ensure XpmCreateDataFromImage validates its conversion inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text()
start = source.index("XpmCreateDataFromImage(\n")
body = source[start:source.index("#undef RETURN", start)]
guard = body.index("if (display == NULL")
before_init = body[guard:body.index("/* initialize return value */", guard)]
assert "data_return == NULL" in before_init
assert "image == NULL && shapeimage == NULL" in before_init
assert "return XpmFileInvalid;" in before_init
print("PASS XpmCreateDataFromImage validates conversion inputs")
