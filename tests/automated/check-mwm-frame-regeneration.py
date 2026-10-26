from pathlib import Path

source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
start = source.index("void RegenerateClientFrame")
end = source.index("END OF FUNCTION  RegenerateClientFrame", start)
body = source[start:end]

assert "pcd == NULL || pcd->clientFrameWin == None" in body
assert "pcd->clientTitleWin != None" in body
assert "pcd->clientBaseWin != None" in body
assert "pcd->clientStretchWin[STRETCH_WEST] != None" in body
print("PASS MWM frame regeneration validates window hierarchy")
