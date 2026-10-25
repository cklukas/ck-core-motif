#!/usr/bin/env python3
"""Check that XmString single-segment extraction validates copied results."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("Boolean\n_XmStringSingleSegment(")
end = source.index("\n/****************************************************************************", start)
region = source[start:end]

assert region.count("XmeStringGetComponent(&stack_context, TRUE, TRUE") >= 2
assert region.count("val == NULL") >= 2
assert "XmStringTag new_tag" in region
assert "if (new_tag == NULL)" in region
assert "retVal = FALSE" in region
print("PASS XmString single-segment copies validate allocation results")
