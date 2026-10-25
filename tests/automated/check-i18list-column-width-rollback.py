#!/usr/bin/env python3
"""Check I18List column-width replacement is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text()
start = source.index("CalcColumnInfo(Widget w, Boolean force)")
end = source.index("\n/*\tFunction Name:", start)
region = source[start:end]

assert "short *new_column_widths;" in region
assert "new_column_widths = (short *) XtMalloc(sizeof(short) * num_cols);" in region
assert "if (new_column_widths == NULL)" in region
assert region.index("new_column_widths = (short *) XtMalloc") < region.index("XtFree((XtPointer) XmI18List_column_widths(ilist))")
assert "XmI18List_column_widths(ilist) = new_column_widths;" in region
assert region.index("if (new_column_widths == NULL)") < region.index("for (i = 0 ; i < num_cols; i++)")
print("I18List preserves column widths on allocation failure")
