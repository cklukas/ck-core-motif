from pathlib import Path

source = Path("clients/mwm/WmManage.c").read_text(encoding="utf-8")
start = source.index("void WithdrawWindow")
end = source.index("END OF FUNCTION WithdrawWindow", start)
body = source[start:end]

assert "if (pCD->clientFrameWin != None)" in body
assert body.index("clientFrameWin != None") < body.index(
    "XUnmapWindow (DISPLAY, pCD->clientFrameWin)"
)
print("PASS MWM unmanage frame withdrawal validates resources")
