#!/usr/bin/env python3
"""Check TextOut navigator reads validate getValue callbacks."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextOut.c").read_text()
assert source.count("nav_trait != NULL && nav_trait->getValue != NULL") >= 2
assert "nav_trait == NULL || nav_trait->getValue == NULL" in source
assert "nav_trait->getValue(w, &nav_data);" in source
