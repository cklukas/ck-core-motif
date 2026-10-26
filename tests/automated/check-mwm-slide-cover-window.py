from pathlib import Path

source = Path("clients/mwm/WmWinState.c").read_text(encoding="utf-8")
start = source.index("pSOR->coverWin = XCreateWindow")
body = source[start:source.index("XConfigureWindow", start)]

assert "if (pSOR->coverWin == None)" in body
assert "pCD->pSOR = NULL" in body
assert "XtFree ((char *)pSOR)" in body
second = source.index("void\nSlideSubpanelBackIn")
second_body = source[second:source.index("XConfigureWindow", second)]
assert "if (pSOR->coverWin == None)" in second_body
assert "pCD->pSOR = NULL" in second_body
print("PASS MWM slide cover-window creation failure is handled")
