#!/usr/bin/env python3
"""Check emitted extended compound-text segment lengths stay representable."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean\nprocessCharsetAndText(")
end = source.index("static Boolean\nctextConcat(", start)
body = source[start:end]

assert "#define CTEXT_EXTENDED_SEGMENT_MAX\t0x3fffU" in source
assert "tag_len > (size_t)UINT_MAX - 8U" in body
assert "*outlen > UINT_MAX - 8U - (unsigned int)tag_len" in body
assert "len >= CTEXT_EXTENDED_SEGMENT_MAX" in body
assert "ctlen > CTEXT_EXTENDED_SEGMENT_MAX - len - 1U" in body
assert "segment_length = len + ctlen + 1U;" in body
assert "(Octet)(0x80U + segment_length / 128U)" in body
print("PASS extended compound-text segment lengths are bounded")
