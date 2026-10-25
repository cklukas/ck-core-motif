#!/usr/bin/env python3
"""Check XmString rendition-tag resizing commits only successful reallocations."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()

assert source.count("int new_count = _XmUnoptSegRendEndCount(entry) + 1;") == 1
assert source.count("int new_count = _XmUnoptSegRendBeginCount(entry) + 1;") == 1
assert source.count("if (new_tags == NULL)\n\t      return;") == 2
assert "_XmUnoptSegRendEndCount(entry) = new_count;" in source
assert "_XmUnoptSegRendBeginCount(entry) = new_count;" in source
assert "_XmUnoptSegRendEnds(entry) = new_tags;" in source
assert "_XmUnoptSegRendBegins(entry) = new_tags;" in source

print("XmString rendition resizing preserves state on allocation failure")
