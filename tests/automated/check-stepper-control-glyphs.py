"""Check Stepper paints distinct centered increment/decrement glyphs."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Stepper.c").read_text(
    encoding="utf-8"
)
redisplay = source[source.index("static void Redisplay(Widget widget") : source.index("static void step(", source.index("static void Redisplay(Widget widget"))]
assert 'const char *symbols[2] = {"+", "−"};' in redisplay
assert "centers[2] = {split + 7, split + 21};" in redisplay
assert "XmStringExtent" in redisplay and "XmStringDraw" in redisplay
print("PASS Stepper uses separate centered increment/decrement glyphs")
