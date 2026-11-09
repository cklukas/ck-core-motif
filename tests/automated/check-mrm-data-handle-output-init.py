#!/usr/bin/env python3
"""Check that failed MRM data-handle operations cannot return stale handles."""

from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Mrm"
checks = {
    "MrmIentry.c": ("Idb__DB_PutDataEntry", "data_entry", "if (file_id == NULL"),
    "MrmIheader.c": ("Idb__HDR_ReturnItem", "entry_return", "if (file_id == NULL)"),
    "MrmIheader.c#put": ("Idb__HDR_PutDataEntry", "data_entry", "if (file_id == NULL"),
    "MrmIindex.c": ("Idb__INX_ReturnItem", "data_entry", "if (file_id == NULL"),
    "MrmIrid.c": ("Idb__RID_ReturnItem", "entry_return", "if (file_id == NULL)"),
}

for source_name, (function, output, fallible_guard) in checks.items():
    source = root / source_name.split("#", 1)[0]
    text = source.read_text(encoding="utf-8")
    start = text.index("\n" + function + " (")
    end = text.find("\n\f", start + 1)
    body = text[start:] if end < 0 else text[start:end]
    null_guard = body.index(f"if ({output} == NULL)")
    clear_record = body.index(f"{output}->rec_no = 0;", null_guard)
    clear_offset = body.index(f"{output}->item_offs = 0;", clear_record)
    first_fallible_guard = body.index(fallible_guard, clear_offset)
    if not (null_guard < clear_record < clear_offset < first_fallible_guard):
        raise SystemExit(f"{function} publishes a stale data handle on failure")

print("MRM data-handle outputs are cleared before fallible operations")
