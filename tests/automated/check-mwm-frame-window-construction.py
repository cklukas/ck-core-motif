from pathlib import Path

source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
start = source.index("Boolean ConstructFrame")
end = source.index("GenerateFrameDisplayLists", start)
body = source[start:end]

assert "if (pcd->clientFrameWin == None)" in body
assert "if (pcd->clientTitleWin == None)" in body
assert "if (pcd->clientBaseWin == None)" in body
assert "XMapSubwindows(DISPLAY, pcd->clientFrameWin);" in body
assert body.index("if (pcd->clientBaseWin == None)") < body.index(
    "XMapSubwindows(DISPLAY, pcd->clientFrameWin)"
)
print("PASS MWM frame construction validates allocated windows")
