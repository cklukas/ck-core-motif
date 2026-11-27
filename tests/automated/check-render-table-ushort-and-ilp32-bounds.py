#!/usr/bin/env python3
"""Verify _XmRenderTableRec unsigned short count caps and ILP32 -Werror=type-limits safety."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

xmrendert = (ROOT / "lib" / "Xm" / "XmRenderT.c").read_text(encoding="utf-8")
assert "(rendition_count > USHRT_MAX)" in xmrendert
assert "_XmRTCount(oldtable) > USHRT_MAX - (unsigned int)count" in xmrendert
assert "if (capacity > USHRT_MAX)" in xmrendert
assert "if (required_count > USHRT_MAX ||" in xmrendert
assert "rc_sz > UINT_MAX / sizeof(Boolean)" not in xmrendert

text_c = (ROOT / "lib" / "Xm" / "Text.c").read_text(encoding="utf-8")
assert "(size_t)nvalid > SIZE_MAX / sizeof(Boolean)" not in text_c
assert "(size_t)nvalid > UINT_MAX / (2 * sizeof(_XmColorEndpoint))" in text_c

xmstring = (ROOT / "lib" / "Xm" / "XmString.c").read_text(encoding="utf-8")
assert "#if SIZE_MAX <= UINT_MAX\n\t  (size_t)c_length > SIZE_MAX - sizeof(wchar_t)\n#else" in xmstring
assert "#if SIZE_MAX <= UINT_MAX\n\t  (size_t)c_length >= SIZE_MAX\n#else" in xmstring
assert "#if SIZE_MAX <= UINT_MAX\n\t          (size_t)char_count > SIZE_MAX - sizeof(wchar_t)\n#else" in xmstring

statusbar = (ROOT / "lib" / "Xm" / "StatusBar.c").read_text(encoding="utf-8")
toolbar = (ROOT / "lib" / "Xm" / "Toolbar.c").read_text(encoding="utf-8")
assert "uintmax_t item_count;" in statusbar
assert "uintmax_t action_count;" in toolbar

print("PASS render-table USHRT_MAX caps and ILP32 -Werror=type-limits guards")
