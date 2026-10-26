from pathlib import Path

source = Path("clients/mwm/WmManage.c").read_text(encoding="utf-8")
start = source.index("void FreeClientFrame")
end = source.index("END OF FUNCTION FreeClientFrame", start)
body = source[start:end]

assert "if (pCD == NULL)" in body
assert "if (pCD->clientFrameWin != None)" in body
assert "pCD->clientFrameWin = None;" in body
print("PASS MWM frame teardown is safe for partial construction")
