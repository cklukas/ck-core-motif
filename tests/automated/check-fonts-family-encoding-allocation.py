#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/FontS.c").read_text()
start = source.index("fam->encoding_alloc = NUM_BUTTONS;")
end = source.index("/*\tFunction Name: SizeMapping", start)
region = source[start:end]
assert "XtCalloc(fam->encoding_alloc" in region
assert "if (fam->encodings == NULL)" in region
assert "fam->encoding_alloc = 0;" in region
assert "return;" in region
print("Font family encoding allocation guard present")
