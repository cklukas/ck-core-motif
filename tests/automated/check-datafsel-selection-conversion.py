#!/usr/bin/env python3
"""Check DataField selection export detects wide conversion failures."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataFSel.c").read_text()
start = source.index("if (*target == XA_STRING)")
end = source.index("} else if (*target == TEXT", start)
first = source[start:end]
second_start = end
second_end = source.index("} else if (*target == COMPOUND_TEXT", second_start)
second = source[second_start:second_end]
third_start = second_end
third_end = source.index("  /*\n   * Provide data for INSERT_SELECTION", third_start)
third = source[third_start:third_end]

for region in (first, second, third):
    assert "size_t stat;" in region
    assert "stat == (size_t)-1" in region

print("PASS DataField selection export detects conversion failure")
