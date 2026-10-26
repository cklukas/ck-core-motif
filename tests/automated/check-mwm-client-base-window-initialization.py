from pathlib import Path

source = Path("clients/mwm/WmWinInfo.c").read_text(encoding="utf-8")
start = source.index("GetClientInfo (WmScreenData")
body = source[start:source.index("END OF FUNCTION GetClientInfo", start)]

assert "pCD->clientBaseWin = None;" in body
assert body.index("clientBaseWin = None") < body.index("return (pCD);")
print("PASS MWM client base window initializes to None")
