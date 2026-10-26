#!/usr/bin/env python3
"""Check checked byte offsets in the legacy Text source gap paths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
assert "static Boolean TextSourceByteOffset" in source
helper = source[source.index("TextSourceByteOffset(XmSourceData"):source.index("/* Convert a stream", source.index("TextSourceByteOffset(XmSourceData"))]
assert "position < 0" in helper
assert "(size_t)position > SIZE_MAX / unit" in helper
set_region = source[source.index("_XmStringSourceSetGappedBuffer"):source.index("/* The only caller", source.index("_XmStringSourceSetGappedBuffer"))]
read_region = source[source.index("_XmStringSourceReadString"):source.index("/* Caller wants block", source.index("_XmStringSourceReadString"))]
get_region = source[source.index("_XmStringSourceGetChar"):source.index("/* Inspect one native source character", source.index("_XmStringSourceGetChar"))]
assert "TextSourceByteOffset(data, position, char_size" in set_region
assert "size_t count, byte_position" in set_region
assert "TextSourceByteOffset(data, (XmTextPosition)start, unit" in read_region
assert "int byte_start" not in read_region
assert "TextSourceByteOffset(data, position, unit" in get_region
assert "register XmTextPosition char_pos" not in get_region
replace_start = source.index("static XmTextStatus \nReplace")
replace_region = source[replace_start:source.index("  /* add data */", replace_start)]
assert "size_t gap_size, gap_start_offset, gap_end_offset, end_byte_offset" in replace_region
assert "TextSourceByteOffset(data, *end" in replace_region
assert "(size_t)old_maxlength * (size_t)char_size" in replace_region
assert "(size_t)data->length * (size_t)char_size" in source
assert "(size_t)(data->maxlength - 1) * (size_t)char_size" in source
print("PASS Text source byte offsets are checked and widened")
