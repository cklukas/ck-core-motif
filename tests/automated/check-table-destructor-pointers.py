#!/usr/bin/env python3
"""Check table converter destructors tolerate null table payloads."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
for function, marker in (
    ("XmStringCvtDestroy", "XmString *table ="),
    ("StringCvtDestroy", "String *table ="),
):
    start = source.rindex(function + "(")
    end = source.index("/****************", start + 1) if function == "XmStringCvtDestroy" else source.index("/*ARGSUSED*/", start + 1)
    region = source[start:end]
    assignment = region.index(marker)
    assert "if (table == NULL)" in region[assignment:]
print("PASS table converter destructors guard null payloads")
