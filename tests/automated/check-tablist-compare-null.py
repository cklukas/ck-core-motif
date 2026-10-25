#!/usr/bin/env python3
"""Check TabList comparison handles absent lists without dereferencing them."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabList.c").read_text()
start = source.index("XmTabbedStackListCompare(XmTabbedStackList list1")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "if (list1 == NULL || list2 == NULL)" in region
assert "return (list1 == list2) ? XmTAB_CMP_EQUAL : XmTAB_CMP_SIZE;" in region
assert region.index("if (list1 == NULL || list2 == NULL)") < region.index("list1->used")
assert "list1 == NULL && list2 != NULL" not in region
print("PASS TabList NULL comparison")
