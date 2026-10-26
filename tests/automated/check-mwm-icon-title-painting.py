from pathlib import Path

source = Path("clients/mwm/WmIDecor.c").read_text(encoding="utf-8")
for name in ("DrawIconTitle", "RedisplayIconTitle"):
    start = source.index(f"void {name}")
    end = source.index("END OF FUNCTION", start)
    body = source[start:end]
    assert "pcd == NULL || pcd->pSD == NULL" in body
    assert "ACTIVE_PSD == NULL" in body
    assert "ICON_FRAME_WIN(pcd) == None" in body
print("PASS MWM icon-title painting validates lifecycle state")
