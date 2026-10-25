from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()

assert source.count("(short) ((int) ig->rectangle.y + (int) ht + (int) mh)") == 4
assert "ig->rectangle.y + ht + mh" not in source
