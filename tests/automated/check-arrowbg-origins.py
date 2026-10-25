from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ArrowBG.c").read_text()

assert "background_x_offset = ((int) aw->rectangle.x +" in source
assert "background_y_offset = ((int) aw->rectangle.y +" in source
assert "(Position) ((int) aw->rectangle.x +" in source
assert "(Position) ((int) aw->rectangle.y +" in source
