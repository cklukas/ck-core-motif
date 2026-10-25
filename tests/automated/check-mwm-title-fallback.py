#!/usr/bin/env python3
"""Check MWM client-title fallback handles XmString allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinInfo.c"
text = source.read_text(encoding="utf-8")

start = text.index("else if (firstTime)", text.index("ProcessWmWindowTitle (ClientData *pCD"))
body = text[start:text.index("    /*", start)]
assert body.count("if (pCD->clientName)") == 1
assert "pCD->clientTitle = XmStringCreateLocalized(pCD->clientName);" in body
assert "if (!pCD->clientTitle)" in body
assert "pCD->clientTitle = wmGD.clientDefaultTitle;" in body
print("MWM title allocation fallback is guarded")
