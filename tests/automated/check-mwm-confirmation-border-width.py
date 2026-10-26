from pathlib import Path

source = Path("clients/mwm/WmWinInfo.c").read_text(encoding="utf-8")
start = source.index("GetWmClientInfo")
end = source.index("END OF FUNCTION GetWmClientInfo", start)
body = source[start:end]

assert "unsigned int borderWidth;" in body
assert "&borderWidth, &depth" in body
assert "borderWidth > (unsigned int)INT_MAX" in body
assert "pCD->xBorderWidth = (int)borderWidth;" in body
assert "(unsigned int*)&(pCD->xBorderWidth)" not in body
print("PASS MWM confirmation geometry stores a checked border width")
