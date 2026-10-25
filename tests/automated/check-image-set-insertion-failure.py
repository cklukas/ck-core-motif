#!/usr/bin/env python3
"""Check built-in image-cache initialization aborts on bucket failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.rindex("InitializeImageSet( void )")
start = source.rfind("static void", 0, start)
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "if (!_XmAddHashEntry(new_image_set, entries[i]->image_name, entries[i]))" in function
failure = function.index("if (!_XmAddHashEntry")
assert "goto allocation_failure;" in function[failure:]
assert function.index("image_set = new_image_set") > failure
assert "_XmFreeHashTable(new_image_set);" in function
print("PASS built-in image-cache insertion failure is transactional")
