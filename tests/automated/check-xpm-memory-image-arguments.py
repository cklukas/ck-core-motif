#!/usr/bin/env python3
"""Ensure XPM data and buffer constructors validate public arguments."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
for name, functions, input_name in (
    ("XpmCrIFrDat.c", ("XpmCreateImageFromData", "XpmCreateXpmImageFromData"), "data"),
    ("XpmCrIFrBuf.c", ("XpmCreateImageFromBuffer", "XpmCreateXpmImageFromBuffer"), "buffer"),
):
    source = (root / name).read_text()
    for function in functions:
        start = source.index(function + "(\n")
        end = source.find("\nint\n", start + 1)
        body = source[start:] if end < 0 else source[start:end]
        guard = body.index("if (")
        before_init = body[guard:body.index("xpmInitXpmImage", guard)]
        assert input_name + " == NULL" in before_init
        assert "return XpmFileInvalid;" in before_init
        if function.startswith("XpmCreateImageFrom"):
            assert "display == NULL" in before_init
            assert "image_return == NULL" in before_init
        else:
            assert "image == NULL" in before_init
            assert "info == NULL" in before_init
print("PASS XPM memory image constructors validate public arguments")
