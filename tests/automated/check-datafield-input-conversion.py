#!/usr/bin/env python3
"""Check DataField keyboard insertion rejects invalid multibyte input."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
for name in ("df_InsertChar", "df_InsertString"):
    start = source.index(name + "(\n        Widget")
    end = source.index("/* ARGSUSED */\nstatic void", start)
    body = source[start:end]
    assert "num_chars = mbstowcs" in body
    assert "num_chars == (size_t)-1" in body
    assert "_XmDataFieldDrawInsertionPoint(tf, True);" in body

print("PASS DataField keyboard conversion rejects invalid multibyte input")
