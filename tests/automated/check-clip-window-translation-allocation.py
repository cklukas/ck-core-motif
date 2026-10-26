#!/usr/bin/env python3
"""Check ClipWindow translation-buffer growth is failure-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ClipWindow.c").read_text()
append = source[source.index("AppendTranslationText"):source.index("static String\nGetRealTranslations")]
get = source[source.index("GetRealTranslations"):source.index("static XtTranslations")]
assert "if (next == 0)" in append
assert "*used == SIZE_MAX" in append
assert "required > (size_t)UINT_MAX" in append
assert "String new_buffer" in append
assert "new_buffer = (String)XtRealloc" in append
assert "if (new_buffer == NULL)" in append
assert "*buffer = new_buffer" in append
assert "if (buf == NULL)" in get
assert "if (buf == NULL)" in get[get.index("XtMalloc"):get.index("buf[0]")]
assert "return NULL;" in get
print("PASS ClipWindow translation growth handles allocation failure")
