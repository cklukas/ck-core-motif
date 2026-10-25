from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "MenuShell.c").read_text()
start = source.index("static void \nPopdownKids")
end = source.index("/*\n * This routine determines", start)
body = source[start:end]

assert "if (subShell->composite.num_children == 0)" in body
