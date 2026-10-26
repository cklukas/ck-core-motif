#!/usr/bin/env python3
"""Ensure XpmReadFileToImage rejects invalid public arguments early."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmRdFToI.c").read_text()
start = source.index("XpmReadFileToImage(\n")
body = source[start:source.index("XpmReadFileToXpmImage(", start)]
guard = body.index("if (display == NULL")
assert "filename == NULL" in body[guard:body.index("xpmInitXpmImage", guard)]
assert "image_return == NULL" in body[guard:body.index("xpmInitXpmImage", guard)]
assert "return XpmFileInvalid;" in body[guard:body.index("xpmInitXpmImage", guard)]
print("PASS XpmReadFileToImage validates public arguments")
