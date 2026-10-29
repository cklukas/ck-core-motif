#!/usr/bin/env python3
"""Ensure Tree centering differences are evaluated as signed geometry."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Tree.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nLayoutChildren")
end = source.index("static Boolean\nGetNodeHeightAndWidth", start)
body = source[start:end]

assert "int box_space;" in body
assert "(int)XmTreeC_bb_height(t_node) + (int)extra_space" in body
assert "(int)XmTreeC_bb_width(t_node) + (int)extra_space" in body
assert "((int)c_height - (int)oc_height) / 2" in body
assert "((int)c_width - (int)oc_width) / 2" in body
assert "(int)XmTreeC_bb_width(*child) -" in source
assert "XmTreeC_bb_height(t_node) + extra_space - c_height" not in body
assert "XmTreeC_bb_width(t_node) + extra_space - c_width" not in body
assert "XmTreeC_bb_width(*child) - XmTree_horizontal_delta(tw)" not in source
print("PASS Tree centering geometry uses signed differences")
