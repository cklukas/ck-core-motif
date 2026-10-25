#!/usr/bin/env python3
"""Check AT-SPI D-Bus memo strings are replaced transactionally."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AccessibleDBus.c").read_text()
helper_start = source.index("static Boolean memo_replace")
helper_end = source.index("static void path_for", helper_start)
helper = source[helper_start:helper_end]
assert "char *replacement = value ? XtNewString(value) : NULL" in helper
assert "if (value && !replacement)" in helper
assert "XtFree(*slot)" in helper
assert "*slot = replacement" in helper

assert source.count("memo_replace(&record->name, text(info.name))") == 3
assert source.count("memo_replace(&record->value_text, text(info.value_text))") == 2
assert "XtFree(record->name); record->name = XtNewString" not in source
assert "XtFree(record->value_text); record->value_text = XtNewString" not in source
print("Accessibility D-Bus memo strings preserve state on allocation failure")
