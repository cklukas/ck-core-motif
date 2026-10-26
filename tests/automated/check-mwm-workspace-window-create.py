from pathlib import Path

source = Path("clients/mwm/WmInitWs.c").read_text(encoding="utf-8")
start = source.index("void SetupWmWorkspaceWindows")
body = source[start:source.index("END OF FUNCTION", start)]

assert "pSD->wmWorkspaceWin = XCreateWindow" in body
assert "if (pSD->wmWorkspaceWin == None)" in body
assert body.index("wmWorkspaceWin == None") < body.index("XMapWindow")
print("PASS MWM workspace helper window creation failure is handled")
