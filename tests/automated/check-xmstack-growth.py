#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/xmlist.c").read_text()
assert "#include <stdint.h>" in source
assert "stack->top == INT_MAX" in source
assert "stack->alloc > INT_MAX - STACK_INC" in source
assert "SIZE_MAX / sizeof(XtPointer)" in source
assert "UINT_MAX / sizeof(XtPointer)" in source
assert "int new_alloc;" in source
assert "stack->alloc = new_alloc;" in source
print("XmStack growth bounds present")
