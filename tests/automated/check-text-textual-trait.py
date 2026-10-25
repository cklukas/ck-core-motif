#!/usr/bin/env python3
"""Check Text public string helpers validate textual trait callbacks."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Text.c").read_text()
assert source.count("textT != NULL && textT->getValue != NULL") >= 2
assert source.count("textT != NULL && textT->setValue != NULL") >= 2
