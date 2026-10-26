from pathlib import Path

source = Path("clients/mwm/WmWinInfo.c").read_text(encoding="utf-8")
start = source.index("GetWmClientInfo")
end = source.index("END OF FUNCTION GetWmClientInfo", start)
body = source[start:end]

assert "if (!XGetGeometry" in body
assert "Could not query confirmation window geometry" in body
assert body.index("if (!XGetGeometry") < body.index("pCD->clientWidth = cbWidth")
print("PASS MWM confirmation geometry query checks status")
