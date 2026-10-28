#!/usr/bin/env python3
"""Check accessibility child indices preserve their signed ABI safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c").read_text()

assert "static int bounded_child_index(size_t value)" in source
assert "value > (size_t)INT_MAX ? INT_MAX : (int)value" in source
assert "return n && n->parent ? bounded_child_index(n->child_index) : -1;" in source
assert "bounded_child_index(n->child_index));" in source
assert "return n && n->parent ? (int)n->child_index : -1;" not in source
print("accessibility child indices clamp at the signed ABI boundary")
