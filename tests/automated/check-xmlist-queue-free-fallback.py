#!/usr/bin/env python3
"""Check queue destruction frees blocks when its cleanup stack is unavailable."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "xmlist.c").read_text()
start = source.index("void\n_XmQueueFree")
end = source.index("/*\tFunction Name: QueuePush", start)
body = source[start:end]

assert "teardown must not depend on another" in body
assert "_XmStackInit" not in body
assert body.count("if (elem->alloced)") == 1
assert "XtFree((XtPointer) elem);" in body
print("queue cleanup has an allocation-free fallback")
