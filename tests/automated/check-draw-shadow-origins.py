from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Draw.c").read_text()
start = source.index("DrawSimpleShadow (")
end = source.index("/**************************** Public functions", start)
body = source[start:end]

assert "int ix = (int) x;" in body
assert "int iy = (int) y;" in body
assert "int iw = (int) width;" in body
assert "int ih = (int) height;" in body
assert "x + width - i - 1" not in body
assert "y + height - i - 1" not in body
