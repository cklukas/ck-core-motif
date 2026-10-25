from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DrTog.c").read_text()

check_start = source.index("static void\nDrawCheckMark(")
check_end = source.index("\f", check_start)
check = source[check_start:check_end]
cross_start = source.index("static void\nDrawCross(")
cross_end = source.index("\f", cross_start)
cross = source[cross_start:cross_end]

assert "int inner_width = (int) width - 2 * (int) margin - 1;" in check
assert "int inner_height = (int) height - 2 * (int) margin - 1;" in check
assert "if (inner_width < 1 || inner_height < 1)" in check
assert "int left   = (int) x + (int) margin;" in cross
assert "int right  = (int) x + (int) width - (int) margin - 1;" in cross
assert "(int) width <= 2 * (int) margin" in cross
