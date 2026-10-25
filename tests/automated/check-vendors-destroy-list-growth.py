#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/VendorS.c").read_text()
assert "#include <limits.h>" in source
assert "destroy_list_size > USHRT_MAX - 2" in source
assert "SIZE_MAX / sizeof(XmDestroyGrabRec)" in source
assert "XmDestroyGrabList new_list" in source
assert "if (new_list == NULL)" in source
assert "destroy_list_size = new_size;" in source
print("VendorS destroy-grab list growth guards present")
