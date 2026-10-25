#!/usr/bin/env python3
"""Check ComboBox action defaults do not assert on invalid values."""

from pathlib import Path


source = Path("lib/Xm/ComboBox.c").read_text()
start = source.rindex("CBListAction(Widget")
end = source.index("\n/*\n * PopdownList", start)
region = source[start:end]

assert "case UP:" in region
assert "case PREVPAGE:" in region
assert "assert(FALSE);" not in region
assert region.count("default:") >= 2

print("ComboBox invalid action values fall back safely")
