"""Check XmScreen SetValues tolerates an unavailable font resource."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c").read_text(
    encoding="utf-8"
)

marker = "if (newScr->screen.font_struct != oldScr->screen.font_struct &&"
assert marker in source
region = source[source.index(marker) : source.index("return FALSE", source.index(marker))]
assert "!newScr->screen.font_struct || !oldScr->screen.font_struct" in region
assert "newScr->screen.font_struct->fid != oldScr->screen.font_struct->fid" in region
print("PASS XmScreen SetValues guards unavailable font resources")
