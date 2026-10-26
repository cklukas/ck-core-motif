#!/usr/bin/env python3
"""Check TextField replacement growth rejects unrepresentable capacities."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c"
text = source.read_text(encoding="utf-8")
start = text.index("_XmTextFieldReplaceText(")
end = text.index("\n}\n", start)
first = text[start:end]
start = text.index("_XmTextFieldReplaceTextForPreedit(")
end = text.index("\n}\n", start)
second = text[start:end]
region = first + second

if region.count("tf->text.size_allocd > UINT_MAX / 2") != 4:
    raise SystemExit("replacement growth lacks capacity overflow guards")
if region.count("new_size_allocd > UINT_MAX / sizeof(wchar_t)") != 2:
    raise SystemExit("wide replacement growth lacks byte-width guards")

print("TextField replacement growth rejects unrepresentable capacities")
