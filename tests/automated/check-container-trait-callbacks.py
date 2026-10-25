#!/usr/bin/env python3
"""Check Container trait callback guards remain adjacent to their call sites."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
for callback in ("getValues", "setValues"):
    assert source.count(f"cItemTrait->{callback} == NULL") >= 2
assert "navigatorTrait == NULL || navigatorTrait->getValue == NULL ||" in source
assert "navigatorTrait->setValue == NULL" in source
assert "navigatorTrait->getValue(*nav" in source
assert "navigatorTrait->setValue(*nav" in source
