#!/usr/bin/env python3
"""Ensure XpmWriteFileFromImage validates display and image inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmWrFFrI.c").read_text()
start = source.index("XpmWriteFileFromImage(\n")
body = source[start:source.index("XpmWriteFileFromXpmImage(", start)]
guard = body.index("if (display == NULL")
before_convert = body[guard:body.index("/* create an XpmImage", guard)]
assert "image == NULL && shapeimage == NULL" in before_convert
assert "return XpmFileInvalid;" in before_convert
print("PASS XpmWriteFileFromImage validates conversion inputs")
