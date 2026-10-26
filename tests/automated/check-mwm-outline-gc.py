from pathlib import Path

source = Path("clients/mwm/WmWinConf.c").read_text(encoding="utf-8")

for function in ("FlashOutline", "DrawOutline"):
    start = source.index("void " + function + " (")
    body = source[start:source.index("END OF FUNCTION", start)]
    assert "ACTIVE_PSD == NULL || ACTIVE_PSD->xorGC == (GC)NULL" in body
    assert body.index("xorGC == (GC)NULL") < body.index("DrawSegments")

print("PASS MWM outline drawing validates XOR GC availability")
