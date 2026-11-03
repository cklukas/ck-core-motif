#!/usr/bin/env python3
"""Check Mrm context resize rejects negative resource sizes."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmcontext.c").read_text(
    encoding="utf-8"
)
start = source.index("UrmResizeResourceContext (URMResourceContextPtr")
end = source.index("\n}\n\n\n\f", start)
validation = source[start:end]

guard = "if ( size < 0 || size > MrmMaxResourceSize)"
assert guard in validation
assert validation.index(guard) < validation.index("if (size < (int)sizeof(long))")
assert "MrmTOO_MANY" in validation
assert "requested_size = (MrmSize)size;" in validation
assert "context_id->buffer_size >= requested_size" in validation

print("PASS Mrm context resize rejects negative sizes before allocation")
