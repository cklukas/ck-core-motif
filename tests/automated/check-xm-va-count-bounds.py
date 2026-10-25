#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/VaSimple.c").read_text()
assert "_XmSaturatingIncrement(int *value)" in source
assert "*value < INT_MAX" in source

first = source.index("_XmCountVaList(", source.index("void"))
start = source.index("_XmCountVaList(", first + 1)
region = source[start:source.index("\n}\n", start) + 2]
assert "button_count == NULL || args_count == NULL" in region
assert "_XmSaturatingIncrement(total_count)" in region
assert "_XmSaturatingIncrement(button_count)" in region
assert "_XmSaturatingIncrement(args_count)" in region

nested_start = source.index("_XmCountNestedList(", source.index("_XmCountNestedList(") + 1)
nested = source[nested_start:source.index("\n}\n", nested_start) + 2]
assert "_XmSaturatingIncrement(total_count)" in nested
assert "_XmSaturatingIncrement(typed_count)" in nested

print("VaSimple counters saturate instead of overflowing")
