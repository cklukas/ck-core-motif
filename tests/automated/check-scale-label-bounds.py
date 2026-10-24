#!/usr/bin/env python3
"""Check that Scale value-label formatting uses bounded writes."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "Scale.c").read_text(encoding="utf-8")
assert text.count('char buff[15];') == 4
assert text.count('snprintf(buff, sizeof(buff), "%d%c"') == 5
assert text.count('snprintf(buff, sizeof(buff), "%d"') == 5
assert 'sprintf(buff, "%d%c"' not in text
assert 'sprintf(buff, "%d"' not in text
print("Scale value-label formatting is bounded")
