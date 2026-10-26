#!/usr/bin/env python3
"""Ensure XpmCreateDataFromXpmImage validates its output and image records."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text()
start = source.index("XpmCreateDataFromXpmImage(\n")
body = source[start:]
guard = body.index("if (data_return == NULL")
before_write = body[guard:body.index("*data_return = NULL", guard)]
assert "image == NULL" in before_write
assert "return XpmFileInvalid;" in before_write
print("PASS XpmCreateDataFromXpmImage validates public arguments")
