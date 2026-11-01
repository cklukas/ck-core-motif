"""Check Chip drawing consumes the shared measured layout."""
from pathlib import Path
source=(Path(__file__).parents[2]/"lib/Xm/Chip.c").read_text()
start=source.index("static void Redisplay(Widget widget")
body=source[start:source.index("static void emit",start)]
assert body.index("metrics(chip,&m);layout(chip,&m,&l);") < body.index("XmStringDraw")
assert "MIN(l.remove.width,l.remove.height)" in body
assert "l.remove.x+(l.remove.width-extent)/2" in body
assert "l.remove.y+(l.remove.height-extent)/2" in body
print("PASS Chip draws measured labels and a centered bounded removal mark")
