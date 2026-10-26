from pathlib import Path

source = Path("clients/mwm/WmFunction.c").read_text(encoding="utf-8")
start = source.index("Boolean F_Refresh_Win")
end = source.index("END OF FUNCTION F_Refresh_Win", start)
body = source[start:end]

assert "pCD && pCD->clientBaseWin != None" in body
assert body.index("pCD->clientBaseWin != None") < body.index("XCreateWindow")
print("PASS MWM refresh action guards a cleared client base window")
