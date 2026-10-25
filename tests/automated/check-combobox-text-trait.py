#!/usr/bin/env python3
"""Check ComboBox textual trait callbacks are optional and guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()
start = source.index("GetEditBoxValue(")
end = source.index("/* ------------- CONVENIENCE FUNCTIONS", start)
body = source[start:end]
assert "textTrait != NULL && textTrait->getValue != NULL" in body
assert "textTrait != NULL && textTrait->setValue != NULL" in body
