from pathlib import Path

source = Path("clients/mwm/WmWinInfo.c").read_text(encoding="utf-8")
start = source.index("GetClientInfo (WmScreenData")
body = source[start:source.index("END OF FUNCTION GetClientInfo", start)]

assert "pCD->iconMask = None;" in body
assert "pCD->wShaped = False;" in body
print("PASS MWM client icon mask and shape initialize safely")
