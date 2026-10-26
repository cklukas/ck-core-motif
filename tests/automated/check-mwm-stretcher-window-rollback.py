from pathlib import Path

source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
header = Path("clients/mwm/WmCDecor.h").read_text(encoding="utf-8")

assert "Boolean CreateStretcherWindows (ClientData *pcd);" in header
start = source.index("Boolean CreateStretcherWindows")
end = source.index("END OF FUNCTION  CreateStretcherWindows", start)
body = source[start:end]
assert "if (pcd->clientStretchWin[iWin] == None)" in body
assert "XDestroyWindow (DISPLAY, pcd->clientStretchWin[cleanup])" in body
assert "return (FALSE);" in body
construct = source[source.index("Boolean ConstructFrame"):start]
assert "if (!CreateStretcherWindows (pcd))" in construct
print("PASS MWM stretcher creation rolls back partial resize handles")
