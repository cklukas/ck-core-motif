#!/usr/bin/env python3
"""Check that drop callbacks validate the transfer widget before rebinding."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("SelectionCallbackWrapper(Widget wid")
end = source.index("\n/**********************************************************/", start)
region = source[start:end]
needle = "if (tc -> widget == NULL)"
assert needle in region
assert region.index(needle) < region.index("wid = tc -> widget;")
print("drop callback validates the real transfer widget before rebinding")
