from pathlib import Path

source = Path("clients/mwm/WmManage.c").read_text(encoding="utf-8")
start = source.index("void FreeClientFrame")
end = source.index("END OF FUNCTION FreeClientFrame", start)
body = source[start:end]

destroy = body.index("XDestroyWindow (DISPLAY, pCD->clientFrameWin)")
assert body.index("pCD->clientFrameWin = None;", destroy) > destroy
assert body.index("pCD->clientTitleWin = None;", destroy) > destroy
assert body.index("pCD->clientBaseWin = None;", destroy) > destroy
print("PASS MWM frame teardown clears child IDs")
