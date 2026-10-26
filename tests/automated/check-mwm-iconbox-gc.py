from pathlib import Path

source = Path("clients/mwm/WmIconBox.c").read_text(encoding="utf-8")

for function in ("MakeShrinkWrapIconsGC", "MakeFadeIconGC"):
    start = source.index(function + " (WmScreenData *pSD")
    body = source[start:source.index("END OF FUNCTION", start)]
    guard = "pSD == NULL || pSD->iconAppearance.inactiveGC == (GC)NULL"
    assert guard in body
    assert body.index(guard) < body.index("XCopyGC")

print("PASS MWM icon-box GC setup validates inactive appearance GC")
