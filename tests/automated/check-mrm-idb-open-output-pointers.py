#!/usr/bin/env python3
"""Check Mrm IDB open helpers validate output handles and allocations."""

from pathlib import Path

root = Path(__file__).parents[2]
read = (root / "lib" / "Mrm" / "MrmItop.c").read_text(encoding="utf-8")
write = (root / "lib" / "Mrm" / "MrmItopw.c").read_text(encoding="utf-8")

for source, function, guard in (
    (read, "UrmIdbOpenFileRead", "file_id_return == NULL"),
    (read, "UrmIdbOpenBuffer", "uid_buffer == NULL || file_id_return == NULL"),
    (write, "UrmIdbOpenFileWrite", "file_id_return == NULL"),
):
    start = source.index(function + " (")
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert guard in region, function
    body = region[region.index("{"): ]
    assert body.index(guard) < body.index("*file_id_return"), function

start = write.index("UrmIdbOpenFileWrite (")
end = write.index("\n}\n", start)
region = write[start:end]
assert "filedesc == NULL" in region
print("Mrm IDB open helpers validate output handles and allocation failure")
