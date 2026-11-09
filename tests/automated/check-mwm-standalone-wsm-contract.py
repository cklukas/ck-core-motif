#!/usr/bin/env python3
"""Check standalone MWM_WSM exposes the command/UTM state it uses."""
from pathlib import Path

root = Path(__file__).parents[2]
global_header = (root / "clients/mwm/WmGlobal.h").read_text(encoding="utf-8")
init = (root / "clients/mwm/WmInitWs.c").read_text(encoding="utf-8")

assert global_header.count("defined(MWM_WSM)") >= 3
assert "Widget\tutmShell" in global_header
assert "CmdTree     *cciTree" in global_header
assert '# include "WmWsmLib/utm_send.h"' in init
assert "pSD->cciTree = NULL;" in init
assert "pSD->utmShell = XmCreateDrawingArea" in init
print("PASS standalone MWM_WSM owns its command and UTM state")
