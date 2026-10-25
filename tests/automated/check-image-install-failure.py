#!/usr/bin/env python3
"""Check image installation rolls back when hash insertion fails."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("_XmInstallImage(")
start = source.index("Boolean", start - 80)
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "if (!_XmAddHashEntry(image_set, entry->image_name, entry))" in function
failure = function.index("if (!_XmAddHashEntry")
assert failure < function.index("_XmProcessUnlock();", failure)
assert "XtFree(entry->image_name);" in function[failure:]
assert "XtFree((char *)entry);" in function[failure:]
assert "return (False);" in function[failure:]
print("PASS image installation rolls back hash insertion failure")
