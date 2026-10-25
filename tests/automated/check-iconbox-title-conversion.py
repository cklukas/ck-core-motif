#!/usr/bin/env python3
"""Check icon-box shell title resources require successful conversion."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

needle = "pchIBTitle = WmXmStringToString (pWS->pSD->iconBoxTitle);"
assert needle in text
start = text.index(needle)
end = text.index("    XtSetArg (setArgs[i], XmNmappedWhenManaged", start)
guard = text[start:end]
assert "if (pchIBTitle != NULL)" in guard
assert guard.count("XtSetArg (setArgs[i], XmNtitle") == 1
assert guard.count("XtSetArg (setArgs[i], XmNiconName") == 1
print("icon-box title resources are guarded")
