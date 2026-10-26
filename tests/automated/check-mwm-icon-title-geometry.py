from pathlib import Path

source = Path("clients/mwm/WmIDecor.c").read_text(encoding="utf-8")
start = source.index("void GetIconTitleBox")
end = source.index("END OF FUNCTION GetIconTitleBox", start)
body = source[start:end]

assert "pcd == NULL || pBox == NULL || pcd->pSD == NULL" in body
assert body.index("pcd == NULL") < body.index("P_ICON_BOX(pcd)")
print("PASS MWM icon-title geometry validates inputs")
