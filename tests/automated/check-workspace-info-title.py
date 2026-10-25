#!/usr/bin/env python3
"""Check workspace title conversion failures are handled before Xlib."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmProperty.c"
text = source.read_text(encoding="utf-8")

needle = "sTitle = (String) WmXmStringToString (pWS->title);"
assert needle in text
start = text.index(needle)
guard = text[start:text.index("ppchList[i++] = (char *) sTitle;", start)]
assert "if (sTitle == NULL)" in guard
assert "Warning" in guard
assert "XtFree ((char *) ppchList);" in guard
assert "XtFree (pch);" in guard
assert "return;" in guard
print("workspace title conversion failures are handled")
