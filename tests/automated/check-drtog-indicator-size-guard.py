from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DrTog.c").read_text()
start = source.index("XmeDrawIndicator(Display *display")
end = source.index("void\nXmeDrawCircle", start)
body = source[start:end]

assert "width <= 0 || height <= 0" in body
