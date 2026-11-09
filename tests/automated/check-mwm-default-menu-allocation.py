#!/usr/bin/env python3
"""Check that MWM localized default-menu construction has a fallback."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text(encoding="utf-8")
fallback_start = source.index("UseBuiltinSystemMenuFallback(void)")
fallback_end = source.index("void InitBuiltinSystemMenu(void)", fallback_start)
fallback = source[fallback_start:fallback_end]
start = source.index("void InitBuiltinSystemMenu(void)")
end = source.index("\n}", start) + 2
region = source[start:end]
assert "XtNewString" not in fallback
assert "DEFAULT_DTWM_SYSTEMMENU" in fallback
assert "BUILTINSYSTEMMENU" in fallback
assert region.count("UseBuiltinSystemMenuFallback();") == 3
assert "(size_t)dsm_length >= UINT_MAX" in region
assert "dsm = XtMalloc((Cardinal)dsm_length + 1);" in region
assert "dsm = XtMalloc((size_t)dsm_length + 1);" not in region
assert "if (dsm == NULL)" in region
assert "goto dsm_done;" in region
assert "dsm_done:" in region
print("PASS MWM default-menu allocation has a non-allocating fallback")
