#!/usr/bin/env python3
"""Check ComboBox child-argument filtering has an allocation fallback."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()
start = source.index("/* Ignore XmNheight resource value")
body = source[start:source.index("#else", start)]
assert "child_args != NULL || *num_args == 0" in body
assert "CreateChildren(new_w, args, num_args);" in source
assert "Cardinal i;" in source
assert "SIZE_MAX / sizeof(Arg)" in body
assert "sizeof(Arg) * (size_t)*num_args" in body
