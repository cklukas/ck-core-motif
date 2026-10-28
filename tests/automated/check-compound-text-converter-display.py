#!/usr/bin/env python3
"""Check public compound-text converters validate their display input."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/ResEncod.c").read_text()
text_start = source.index("XmCvtTextToXmString(")
text_end = source.index("    ok = cvtTextToXmString", text_start)
text_body = source[text_start:text_end]
inverse_start = source.index("XmCvtXmStringToText(")
inverse_end = source.index("    ok = cvtXmStringToText", inverse_start)
inverse_body = source[inverse_start:inverse_end]

assert "display == NULL" in text_body
assert "display == NULL" in inverse_body

print("PASS compound-text converters validate display input")
