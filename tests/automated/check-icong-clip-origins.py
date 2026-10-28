from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()

assert source.count("bounded_position((long long)ig->rectangle.y + ht + mh)") >= 1
assert "XSetClipOrigin" in source
