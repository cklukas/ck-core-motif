from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DrArrow.c").read_text()

assert "width < 3 || height < 3" in source
