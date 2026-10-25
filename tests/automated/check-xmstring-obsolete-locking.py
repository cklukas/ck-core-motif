#!/usr/bin/env python3
"""Check the legacy L-to-R constructor avoids recursive locking."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringObso.c").read_text()
start = source.index("XmStringLtoRCreate(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "_XmProcessLock();" not in function
assert "_XmProcessUnlock();" not in function
assert "XmStringDirectionCreate" in function
assert "XmStringConcatAndFree" in function
assert "XmStringSeparatorCreate" in function

print("PASS legacy XmString L-to-R creation avoids recursive process locking")
