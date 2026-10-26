from pathlib import Path

source = Path("clients/mwm/WmWinInfo.c").read_text(encoding="utf-8")
start = source.index("GetWmClientInfo")
end = source.index("END OF FUNCTION GetWmClientInfo", start)
body = source[start:end]

assert "cbWidth > (unsigned int)INT_MAX" in body
assert "cbHeight > (unsigned int)INT_MAX" in body
guard = body.index("cbWidth > (unsigned int)INT_MAX")
assignment = body.index("pCD->clientWidth = cbWidth")
assert guard < assignment
print("PASS MWM confirmation geometry rejects unrepresentable dimensions")
