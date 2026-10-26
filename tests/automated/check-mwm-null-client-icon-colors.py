from pathlib import Path

source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("Pixmap MakeIconPixmap")
end = source.index("END OF FUNCTION MakeIconPixmap", start)
body = source[start:end]

assert "pCD ? ICON_APPEARANCE(pCD).background" in body
assert "pSD->iconAppearance.background" in body
assert "pCD ? ICON_APPEARANCE(pCD).foreground" in body
assert "pSD->iconAppearance.foreground" in body
print("PASS WSM null-client icon colors use screen defaults")
