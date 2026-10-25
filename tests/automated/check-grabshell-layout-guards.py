from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "GrabShell.c").read_text()
start = source.index("DoLayout(Widget wid)")
end = source.index("/************************************************************************\n *\n *  GeometryManager", start)
body = source[start:end]

assert "if (gs->composite.num_children == 0)" in body
assert "int offset;" in body
assert "(int) gs->grab_shell.shadow_thickness" in body
