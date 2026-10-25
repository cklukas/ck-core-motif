#!/usr/bin/env python3
"""Check that tab-list proposal rejects string-context traversal failures."""

from pathlib import Path


source = Path("lib/Xm/XmStringFunc.c").read_text()
start = source.index("XmStringTableProposeTablist(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (_XmStrContError(&stack_ctx))" in function
failure = function.index("if (_XmStrContError(&stack_ctx))")
cleanup = function[failure:]
assert "_XmStringContextFree(&stack_ctx);" in cleanup
assert "XmTabListFree(tl);" in cleanup
assert "return ((XmTabList)NULL);" in cleanup

print("XmString tab-list proposal rejects traversal context failures")
