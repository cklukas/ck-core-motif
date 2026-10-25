#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/PushBG.c").read_text()
start = source.index("static void \nClassInitialize( void )")
end = source.index("static void \nClassPartInitialize(", start)
region = source[start:end]
assert "size_t\t merged_count" in region
assert "SIZE_MAX / sizeof(XtResource)" in region
assert "UINT_MAX / sizeof(XtResource)" in region
assert "merged_list = (XtResource *)XtMalloc" in region
assert "if (merged_list == NULL)" in region
assert region.index("if (merged_list == NULL)") < region.index("merged_list[i] =")
print("PushButtonGadget resource-list allocation guard present")
