#!/usr/bin/env python3
"""Ensure XpmWriteFileFromXpmImage validates its source image."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmWrFFrI.c").read_text()
start = source.index("XpmWriteFileFromXpmImage(\n")
body = source[start:]
guard = body.index("if (image == NULL")
before_open = body[guard:body.index("/* open file to write */", guard)]
assert "return XpmFileInvalid;" in before_open
print("PASS XpmWriteFileFromXpmImage validates source image")
