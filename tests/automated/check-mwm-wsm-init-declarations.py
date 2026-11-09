#!/usr/bin/env python3
"""Check WSM initialization includes its direct CDE/property declarations."""
from pathlib import Path

root = Path(__file__).parents[2]
source = (root / "clients/mwm/WmInitWs.c").read_text(encoding="utf-8")
header = (root / "clients/mwm/WmProperty.h").read_text(encoding="utf-8")

assert "#include <Dt/EnvControlP.h>" in source
assert "GetDtSessionHints (WmData" not in source
assert "GetDtSessionHints (WmScreenData *pSD, int sNum);" in header
assert "pSD->lastBackdropWin = None;" in source
print("PASS MWM WSM initialization declares backdrop and session helpers")
