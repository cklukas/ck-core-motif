#!/usr/bin/env python3
"""Check WML class-mask emitters stream terms instead of concatenating them."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmlouth.c").read_text(
    encoding="utf-8"
)

helper_start = source.index("static void\nwmlOutputClassMaskTerm")
helper_end = source.index("void wmlOutputUilSymCSet", helper_start)
helper = source[helper_start:helper_end]
assert "fprintf (outfil, \"%s _BIT_MASK(sym_k_%s_object) |\\n \"" in helper

for function, next_function in (
    ("void wmlOutputUilSymReas ()", "void wmlOutputUilSymArTa ()"),
    ("void wmlOutputUilSymArTa ()", "void wmlOutputUilSymChTa ()"),
    ("void wmlOutputUilSymChTa ()", "void wmlOutputUilSymCtl ()"),
    ("void wmlOutputUilSymCtl ()", "int wmlResolveCtlIsMember"),
):
    start = source.index(function)
    end = source.index(next_function, start)
    body = source[start:end]
    assert "maskbuf[500]" not in body
    assert "itembuf[100]" not in body
    assert "sprintf (itembuf" not in body
    assert "strcat (maskbuf" not in body
    assert "wmlOutputClassMaskTerm (outfil, mask_prefix," in body
    assert "mask_prefix = \" \";" in body
    assert "mask_prefix = \",\";" in body

print("PASS WML class-mask emitters stream unbounded class names")
