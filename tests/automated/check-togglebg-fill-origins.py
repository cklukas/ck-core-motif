from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleBG.c").read_text()

def function_body(name, next_name):
    start = source.index("static void \n" + name)
    end = source.index("static void \n" + next_name, start)
    return source[start:end]


label = function_body("DrawToggleLabel", "DrawEtchedInMenu")
etched_start = source.index("static void \nDrawEtchedInMenu")
etched = source[etched_start:source.index("\f", etched_start + 20)]

for body in (label, etched):
    assert "int margin = (int) tb->gadget.highlight_thickness +" in body
    assert "int fx = (int) tb->rectangle.x + margin;" in body
    assert "int fy = (int) tb->rectangle.y + margin;" in body
