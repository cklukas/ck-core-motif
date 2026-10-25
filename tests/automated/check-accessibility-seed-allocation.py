#!/usr/bin/env python3
"""Check that initial accessibility tree seeding handles snapshot failure."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "AccessibleDBus.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void seed_connected_tree(")
end = text.index("\nstatic ", start + 1)
region = text[start:end]
assert "ids = (XmAccessibleId *)XtMalloc" in region
assert "if (!ids) goto done;" in region
print("accessibility tree seeding guards snapshot allocation")
