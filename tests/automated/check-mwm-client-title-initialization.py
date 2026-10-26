from pathlib import Path

source = Path("clients/mwm/WmWinInfo.c").read_text(encoding="utf-8")
start = source.index("GetClientInfo (WmScreenData")
body = source[start:source.index("END OF FUNCTION GetClientInfo", start)]

assert "pCD->clientTitle = NULL;" in body
assert "pCD->iconTitle = NULL;" in body
print("PASS MWM client title handles initialize to NULL")
