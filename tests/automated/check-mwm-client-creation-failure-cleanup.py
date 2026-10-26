from pathlib import Path

source = Path("clients/mwm/WmWinInfo.c").read_text(encoding="utf-8")
start = source.index("GetWmClientInfo (")
body = source[start:source.index("END OF FUNCTION GetWmClientInfo", start)]

assert '"Couldn\'t make icon box"' in body
assert '"Could not query confirmation window geometry"' in body
assert body.count("XtFree ((char *)pCD)") >= 2
print("PASS MWM client creation failures release client data")
