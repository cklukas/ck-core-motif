#!/usr/bin/env python3
"""Check tab-list label updates are transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabList.c").read_text()
start = source.index("XmTabbedStackListModify(XmTabbedStackList tab_list")
end = source.index("/*\n * Function:", start)
region = source[start:end]

assert "XmString replacement = NULL;" in region
assert "if (attributes->label_string != NULL && replacement == NULL)" in region
assert "if( (mask & XmTAB_LABEL_STRING) && !switch_to_copy )" in region
assert region.index("replacement = XiXmStringCopy(attributes->label_string);") < region.index("XiXmStringFree(tab->label_string);")
assert "tab->label_string = replacement;" in region
print("tab-list modification preserves labels on copy failure")
