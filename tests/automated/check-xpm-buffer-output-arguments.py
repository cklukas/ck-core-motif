#!/usr/bin/env python3
"""Ensure XPM buffer-export wrappers validate public arguments."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
checks = (
    ("XpmCrBufFrP.c", "XpmCreateBufferFromPixmap", "buffer_return == NULL", "pixmap == 0 && shapemask == 0"),
    ("XpmCrBufFrI.c", "XpmCreateBufferFromImage", "buffer_return == NULL", "image == NULL && shapeimage == NULL"),
    ("XpmCrBufFrI.c", "XpmCreateBufferFromXpmImage", "buffer_return == NULL", "image == NULL"),
)
for filename, function, output_check, input_check in checks:
    source = (root / filename).read_text()
    start = source.index(function + "(\n")
    body = source[start:]
    guard = body.index("if (")
    before_init = body[guard:body.index("/* get geometry */", guard)] if "/* get geometry */" in body[guard:] else body[guard:body.index("*buffer_return = NULL", guard)]
    assert output_check in before_init
    assert input_check in before_init
    assert "return XpmFileInvalid;" in before_init
print("PASS XPM buffer-export wrappers validate public arguments")
