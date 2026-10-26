from pathlib import Path

source = Path("clients/mwm/WmIDecor.c").read_text(encoding="utf-8")
start = source.index("void ShowActiveIconText")
end = source.index("END OF FUNCTION ShowActiveIconText", start)
body = source[start:end]

assert "pcd == NULL || pcd->pSD == NULL" in body
assert "pcd->pSD->activeIconTextWin == None" in body
assert "ICON_FRAME_WIN(pcd) == None" in body
assert body.index("pcd == NULL") < body.index("XGetWindowAttributes")
print("PASS MWM active icon-label show validates inputs")
