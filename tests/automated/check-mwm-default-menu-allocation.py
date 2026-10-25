#!/usr/bin/env python3
"""Check that MWM localized default-menu construction has a fallback."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text(encoding="utf-8")
start = source.index("void InitBuiltinSystemMenu(void)")
end = source.index("\n}", start) + 2
region = source[start:end]
assert "dsm = XtMalloc((size_t)dsm_length + 1);" in region
assert "if (dsm == NULL)" in region
assert "goto dsm_done;" in region
assert "dsm_done:" in region
assert "XtNewString((String)BUILTINSYSTEMMENU)" in region
print("PASS MWM default-menu allocation has a fallback")
