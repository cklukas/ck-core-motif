from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "MenuUtil.c").read_text()
start = source.index("static Boolean \nFindNextMenuBarCascade(")
body = source[start:]

assert "shell != NULL && shell->composite.num_children > 0" in body
