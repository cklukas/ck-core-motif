from pathlib import Path

source = Path("clients/mwm/WmManage.c").read_text(encoding="utf-8")
start = source.index("void FreeIcon")
end = source.index("END OF FUNCTION FreeIcon", start)
body = source[start:end]

assert "pCD->pWsList != NULL && pCD->pWsList[0].iconFrameWin != None" in body
assert "if (pCD->iconFrameWin != None)" in body
assert "pCD->iconFrameWin = None;" in body
print("PASS MWM icon teardown validates optional storage")
