#!/usr/bin/env python3
"""Check that failed selection ownership preserves the existing context."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("Boolean\nXmeNamedSource(")
end = source.index("/****************************************************************/\n/* XmeSecondarySource", start)
body = source[start:end]

own = body.index("status = XtOwnSelection")
clear = body.index("ClearContextBlock", own)
assert body.index("if (status)", own) < clear
assert body.find("ClearContextBlock", 0, own) == -1

print("PASS XmeNamedSource preserves context until ownership succeeds")
