#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/SelectioB.c").read_text()
for function in ("XmCreateSelectionDialog", "XmCreatePromptDialog"):
    start = source.index(function)
    end = source.index("\n}\n", start) + 2
    region = source[start:end]
    assert "(size_t)sb_n >= SIZE_MAX / sizeof(Arg)" in region
    assert "sb_n >= (Cardinal)(UINT_MAX / sizeof(Arg))" in region
    assert "(sb_n != 0 && sb_args == NULL)" in region
    assert "if (_sb_args == NULL)" in region
    assert "if (sb_n != 0)\n        memcpy(_sb_args, sb_args, sizeof (Arg) * (size_t)sb_n);" in region

print("Selection and prompt dialog argument merges check byte bounds and null lists")
