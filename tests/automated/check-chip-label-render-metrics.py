"""Check Chip measures labels before drawing and centers the close glyph."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Chip.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Redisplay")
body = source[start : source.index("static void emit", start)]
measure = body.index("XmStringExtent(chip->chip.font, chip->chip.label")
draw = body.index("XmStringDraw", measure)
assert measure < draw
assert "int cy = (int)chip->core.height / 2;" in body
assert "cx - 4, cy - 4" in body
assert "cx + 4, cy + 4" in body
print("PASS Chip measures labels and centers a fixed-size close glyph")
