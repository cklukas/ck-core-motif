from pathlib import Path

source = Path("clients/mwm/WmWinInfo.c").read_text(encoding="utf-8")
start = source.index("GetClientInfo (WmScreenData")
body = source[start:source.index("END OF FUNCTION GetClientInfo", start)]

assert "pCD->currentWsc = 0;" in body
print("PASS MWM current workspace index initializes to zero")
