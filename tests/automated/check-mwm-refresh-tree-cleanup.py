from pathlib import Path

source = Path("clients/mwm/WmFunction.c").read_text(encoding="utf-8")
start = source.index("RefreshByClearing (Window win)")
body = source[start:source.index("END OF FUNCTION", start)]

assert "if (winChildren != NULL)" in body
assert body.index("winChildren != NULL") < body.index("XFree((char *)winChildren)")
print("PASS MWM refresh tree cleanup follows returned child-list pointer")
