from pathlib import Path

source = Path("clients/mwm/WmWinConf.c").read_text(encoding="utf-8")
start = source.index("CreateOutlineWindows (WmScreenData *pSD)")
body = source[start:source.index("END OF FUNCTION  CreateOutlineWindows", start)]

assert "woN == None" in body and "woW == None" in body
assert "XDestroyWindow (DISPLAY, pSD->woN)" in body
assert "pSD->woN = pSD->woS = pSD->woE = pSD->woW = None" in body
window = source[source.index("void WindowOutline"):
                 source.index("END OF FUNCTION  WindowOutline")]
assert "if (!CreateOutlineWindows(pSD))" in window
assert "pSD->woS == None || pSD->woE == None || pSD->woW == None" in window
print("PASS MWM outline windows allocate transactionally")
