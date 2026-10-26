from pathlib import Path

source = Path("clients/mwm/WmIDecor.c").read_text(encoding="utf-8")
start = source.index("void PaintActiveIconText")
end = source.index("END OF FUNCTION PaintActiveIconText", start)
body = source[start:end]

assert "pcd == NULL || pcd->pSD == NULL" in body
assert "pcd->pSD->activeIconTextWin == None" in body
assert body.index("pcd == NULL") < body.index("ICON_DECORATION(pcd)")
print("PASS MWM active icon-label painting validates resources")
