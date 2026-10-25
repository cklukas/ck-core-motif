#!/usr/bin/env python3
"""Check List button-transfer defaults do not assert on invalid resources."""

from pathlib import Path


source = Path("lib/Xm/List.c").read_text()
start = source.rindex("ListProcessBtn1(")
end = source.rindex("ListProcessBtn2(", start)
btn1 = source[start:end]
start = source.rindex("ListProcessBtn2(")
end = source.rindex("ListQuickNavigate(", start)
btn2 = source[start:end]

assert "case XmOFF:" in btn1
assert "case XmBUTTON2_ADJUST:" in btn2
assert "assert(FALSE);" not in btn1
assert "assert(FALSE);" not in btn2

print("List button-transfer invalid values fall back safely")
