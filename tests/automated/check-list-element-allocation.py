#!/usr/bin/env python3
"""Check List internal element allocation rolls back safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
allocation = "new_el = (ElementPtr)XtMalloc(sizeof(Element));"
start = source.index(allocation)
guard = source.index("if (new_el == NULL)", start)
rollback = source.index("while (inserted > 0)", guard)
restore = source.index("memmove((char *)(lw->list.InternalList + original_pos)", rollback)
assert start < guard < rollback < restore
assert "lw->list.MaxWidth = old_max_width;" in source[rollback:source.index("return 0;", rollback)]
print("PASS List internal element allocation rollback is present")
