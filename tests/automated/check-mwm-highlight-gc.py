from pathlib import Path

source = Path("clients/mwm/WmResource.c").read_text(encoding="utf-8")
start = source.index("GC GetHighlightGC")
end = source.index("END OF FUNCTION GetHighlightGC", start)
body = source[start:end]

assert "pSD == NULL || pSD->screenTopLevelW == NULL" in body
assert body.index("screenTopLevelW == NULL") < body.index("XtGetGC")
print("PASS MWM highlight GC validates screen state")
