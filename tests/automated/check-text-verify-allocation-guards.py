#!/usr/bin/env python3
"""Ensure TextStrSo modify-verify buffers check allocation results."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c"
text = source.read_text(encoding="utf-8")

char_start = text.index("  if (initiator->text.modify_verify_callback)")
wcs_start = text.index("  if (initiator->text.wcs_modify_verify_callback)", char_start)
char_block = text[char_start:wcs_start]
wcs_end = text.index("  }  /* end if there are wide char modify verify callbacks */", wcs_start)
wcs_block = text[wcs_start:wcs_end]

assert char_block.count("if (newblock->ptr == NULL)") >= 2
assert "if (newblock->length < 0" in wcs_block
assert "if (wcs_newblock.wcsptr == NULL)" in wcs_block
assert "SIZE_MAX / sizeof(wchar_t)" in wcs_block

print("text modify-verify allocation guards: ok")
