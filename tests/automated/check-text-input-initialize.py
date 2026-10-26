#!/usr/bin/env python3
"""Ensure Text input allocation failure cannot silently publish bad state."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmTextInputCreate(Widget wid,")
end = source.index("\nstatic XmTextPosition\nXtoPosInLine", start)
region = source[start:end]

assert "if (input == NULL) {" in region
assert 'XtError("Unable to allocate Text input state");' in region
assert "if (data == NULL) {" in region
assert "tw->text.input = NULL;" in region
assert 'XtError("Unable to allocate Text input data");' in region

print("Text input initialization: allocation failures are explicit and terminal")
