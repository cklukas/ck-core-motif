#!/usr/bin/env python3
"""Check MWM push-button gadget creation before child use."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmMenu.c"
text = source.read_text(encoding="utf-8")

needle = "children[n] = XmCreatePushButtonGadget"
start = text.index(needle)
body = text[start:text.index("            /*", start)]
assert "if (children[n] == NULL)" in body
assert "MWarning" in body
assert "return (NULL);" in body
assert text.count("if (children[n] == NULL)") >= 2
assert "if (children[n - 1] == NULL)" in text
print("MWM push-button gadget creation is guarded")
