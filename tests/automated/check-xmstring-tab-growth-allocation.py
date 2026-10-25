#!/usr/bin/env python3
"""Check proposed XmString tab-list growth rolls back safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("if (j >= _XmTabLCount(tl))")
tab = source.index("tab = XmTabCreate", start)
guard = source.index("if (tab == NULL)", tab)
cleanup = source.index("XmTabListFree(tl);", guard)
assert tab < guard < cleanup
assert "_XmStringContextFree(&stack_ctx);" in source[guard:cleanup]
assert "_XmProcessUnlock();" in source[cleanup:source.index("return ((XmTabList)NULL);", cleanup)]
print("PASS proposed XmString tab-list growth is transactional")
