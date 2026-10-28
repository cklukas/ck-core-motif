#!/usr/bin/env python3
"""Check skipped transfer callbacks release selection-owned values."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("SelectionCallbackWrapper(Widget wid")
end = source.index("/**********************************************************/", start)
body = source[start:end]

assert "Boolean value_delivered = False;" in body
assert "value_delivered = True;" in body
assert "if (!value_delivered)" in body
assert "XtFree((char *)value);" in body
assert body.index("if (!value_delivered)") < body.index("/* Free this transfer block */")

print("PASS skipped transfer callbacks release selection values")
