#!/usr/bin/env python3
"""Check queue destruction frees blocks when its cleanup stack is unavailable."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "xmlist.c").read_text()
start = source.index("void\n_XmQueueFree")
end = source.index("/*\tFunction Name: QueuePush", start)
body = source[start:end]

assert "if (stack != NULL)" in body
assert "else {" in body
assert "The cleanup stack itself can be unavailable" in body
assert body.count("if (elem->alloced)") >= 2
assert "XtFree((XtPointer) elem);" in body
print("queue cleanup has an allocation-free fallback")
