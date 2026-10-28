#!/usr/bin/env python3
"""Check transfer callbacks normalize malformed selection results."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("SelectionCallbackWrapper(Widget wid")
end = source.index("/**********************************************************/", start)
body = source[start:end]

assert "Boolean malformed_result;" in body
assert "*length != 0 && (*type == None || value == NULL)" in body
assert "*length == 0 && value != NULL" in body
assert "XtFree((char *)value);" in body
assert "cbstruct.reason = malformed_result ? XmCR_PROTOCOLS : XmCR_OK;" in body
assert "*length = 0;" in body
assert "*type = None;" in body

print("PASS transfer callbacks normalize malformed selection results")
