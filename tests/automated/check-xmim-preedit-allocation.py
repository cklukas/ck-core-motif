#!/usr/bin/env python3
"""Check XIM preedit allocation failures do not dereference null buffers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()
start = source.index("replacement = (wchar_t *)XtCalloc")
end = source.index("\nstatic void\nImPreeditCaretCallback", start)
region = source[start:end]
assert "if (replacement == NULL || feedback == NULL)" in region
assert region.index("if (replacement == NULL") < region.index("memcpy(replacement")
assert "if (dispatch_characters != NULL)" in region
assert region.index("if (dispatch_characters != NULL)") < region.index("memcpy(dispatch_characters")
print("PASS XmIm preedit allocation failure handling")
