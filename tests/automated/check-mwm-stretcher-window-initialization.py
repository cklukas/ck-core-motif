from pathlib import Path

source = Path("clients/mwm/WmWinInfo.c").read_text(encoding="utf-8")
start = source.index("GetClientInfo (WmScreenData")
body = source[start:source.index("END OF FUNCTION GetClientInfo", start)]

assert "for (i = 0; i < STRETCH_COUNT; i++)" in body
assert "pCD->clientStretchWin[i] = None" in body
print("PASS MWM client stretcher windows initialize to None")
