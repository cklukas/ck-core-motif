from pathlib import Path

source = Path("clients/mwm/WmWinInfo.c").read_text(encoding="utf-8")
start = source.index("WmICCCMToXmString")
end = source.index("ProcessWmWindowTitle", start)
region = source[start:end]

assert "wmNameProp->nitems == 0" in region
assert "strlen((char *)wmNameProp->value)" not in region
print("PASS MWM title-property emptiness uses the X11 item count")
