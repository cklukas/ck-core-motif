#!/usr/bin/env python3
"""Check FontSelector toggle callback state normalization."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()


def function_body(name: str, next_marker: str) -> str:
    start = source.index(f"{name}(Widget w")
    return source[start:source.index(next_marker, start)]


for name, marker in (
    ("ToggleScaling", "/*\tFunction Name: ToggleBold"),
    ("ToggleBold", "/*\tFunction Name: ToggleItalic"),
    ("ToggleItalic", "/*\tFunction Name: ToggleMiddlePane"),
    ("ToggleNameWindow", "/*\tFunction Name: Toggle75DPI"),
):
    body = function_body(name, marker)
    assert "Boolean set;" in body
    assert "set = (info->set != 0) ? True : False;" in body
    normalized = body[body.index("set = (info->set != 0) ? True : False;") :]
    assert "info->set" not in normalized.replace(
        "set = (info->set != 0) ? True : False;", "")

encoding_start = source.index("CheckEncoding(XmFontSelectorWidget fsw")
encoding = source[encoding_start:source.index("/************************************************************", encoding_start)]
assert "return(TRUE);\n\n    if (fam->encoding_alloc <= 0)" in encoding

print("FontSelector toggle callback state is normalized to Boolean values")
