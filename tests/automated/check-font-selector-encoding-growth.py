#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/FontS.c").read_text()
start = source.index("UpdateExistingFamily(FamilyInfo *fam")
end = source.index("FillNewFamily(", start)
region = source[start:end]
assert "fam->encoding_alloc > INT_MAX - NUM_BUTTONS" in region
assert "SIZE_MAX / sizeof(XrmQuark)" in region
assert "UINT_MAX / sizeof(XrmQuark)" in region
assert "XrmQuark *new_encodings" in region
assert "if (new_encodings == NULL)" in region
assert region.index("if (new_encodings == NULL)") < region.index("fam->encodings = new_encodings")
assert "fam->encoding_alloc = new_alloc" in region
print("FontSelector family-encoding growth guards present")
