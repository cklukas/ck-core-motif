from pathlib import Path

source = Path("clients/mwm/WmIDecor.c").read_text(encoding="utf-8")
for name in ("ShowActiveIcon", "ShowInactiveIcon"):
    start = source.index(f"void {name}")
    end = source.index("{", start)
    header = source[start:end + 1]
    body_end = source.index("END OF FUNCTION", start)
    body = source[start:body_end]
    assert "pcd == NULL || pcd->pSD == NULL" in body
    assert "ACTIVE_PSD == NULL" in body
    assert "ICON_FRAME_WIN(pcd) == None" in body
print("PASS MWM icon painting validates lifecycle state")
