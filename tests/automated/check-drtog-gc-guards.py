from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DrTog.c").read_text()

diamond_start = source.index("void XmeDrawDiamond(")
diamond_end = source.index("/******************************XmeDrawIndicator", diamond_start)
diamond = source[diamond_start:diamond_end]
circle_start = source.index("XmeDrawCircle(")
circle = source[circle_start:]

assert "if (!d || !width || !top_gc) return ;" in diamond
assert "if (!bottom_gc)" in diamond
assert "shadow_thick > 0 && (top_gc == NULL || bottom_gc == NULL)" in circle
