#!/usr/bin/env python3
"""Check tab construction and copying handle allocation failures."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c"
text = source.read_text()

create_start = text.index("XmTab\nXmTabCreate")
create_end = text.index("/*ARGSUSED*/", create_start)
create = text[create_start:create_end]
if "if (tab == NULL)" not in create:
    raise SystemExit("XmTabCreate does not guard its record allocation")
if "decimal != NULL && _XmTabDecimal(tab) == NULL" not in create:
    raise SystemExit("XmTabCreate does not roll back decimal allocation failure")

copy_start = text.index("XmTab\n_XmTabCopy")
copy_end = text.index("/***********", copy_start)
copy = text[copy_start:copy_end]
if "if (new_tab == NULL)" not in copy:
    raise SystemExit("_XmTabCopy does not guard its record allocation")
if "XtFree((char *)new_tab);" not in copy:
    raise SystemExit("_XmTabCopy does not release a partial copy")

print("tab construction and copying guard allocation failures")
