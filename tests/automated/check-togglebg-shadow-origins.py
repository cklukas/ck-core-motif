from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleBG.c").read_text()

assert "dx = (int) tb->rectangle.x + (int) hilite_thickness;" in source
assert "dy = (int) tb->rectangle.y + (int) hilite_thickness;" in source
assert "dx = (int)(tb->rectangle.x + hilite_thickness);" not in source
assert "dy = (int)(tb->rectangle.y + hilite_thickness);" not in source
