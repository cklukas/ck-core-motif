from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "MenuShell.c").read_text()

for function in ("static void \n_XmFastPopdown", "static void \nPopdown(",
                 "static void \nPopdownKids"):
    start = source.index(function)
    assert "composite.num_children == 0" in source[start:start + 700]
