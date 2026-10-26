#!/usr/bin/env python3
"""Check that MWM replacement allocation preserves the old block on failure."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmResource.c").read_text(
    encoding="utf-8"
)
start = source.index("char * WmMalloc (ptr, size)")
end = source.index("\n} /* END OF FUNCTION WmMalloc */", start)
region = source[start:end]

assert "char *replacement;" in region
assert "replacement = (char *)XtMalloc (size);" in region
assert "if (replacement == NULL)" in region
assert "return (NULL);" in region
assert "if (ptr)" in region
assert "XtFree (ptr);" in region
assert "return (replacement);" in region
assert region.index("XtMalloc (size)") < region.index("XtFree (ptr)")
print("PASS MWM replacement allocation preserves old storage on failure")
