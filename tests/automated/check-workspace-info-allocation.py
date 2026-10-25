#!/usr/bin/env python3
"""Check workspace property buffers are validated before use."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmProperty.c"
text = source.read_text(encoding="utf-8")

needle = "if (ppchList == NULL || pch == NULL)"
assert needle in text
guard = text[text.index(needle):text.index(needle) + 300]
assert "XtFree ((char *) ppchList);" in guard
assert "XtFree (pch);" in guard
assert "return;" in guard
print("workspace information property allocations are checked")
