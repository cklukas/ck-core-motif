#!/usr/bin/env python3
"""Check Mrm resource-ID helpers validate handles and allocations."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIrid.c").read_text(encoding="utf-8")
assert "(buffer) != NULL && (buffer)->IDB_record != NULL" in source
checks = {
    "Idb__RID_EnterItem": "if (file_id == NULL)",
    "Idb__RID_ReturnItem": "if (entry_return == NULL)",
    "Idb__RID_NextRID": "if (res_id_return == NULL)",
    "Idb__RID_AddRecord": "bufptr == NULL || bufptr->IDB_record == NULL",
}
for function, guard in checks.items():
    start = source.index("\n" + function + " (")
    next_start = source.find("\nCardinal ", start + 1)
    region = source[start:] if next_start < 0 else source[start:next_start]
    assert guard in region, (function, guard)
return_item_start = source.index("\nIdb__RID_ReturnItem (")
return_item_end = source.find("\n\f", return_item_start + 1)
return_item = source[return_item_start:] if return_item_end < 0 else source[return_item_start:return_item_end]
assert return_item.index("entry_return->rec_no = 0;") < return_item.index("if (file_id == NULL)")
assert return_item.index("entry_return->item_offs = 0;") < return_item.index("if (file_id == NULL)")
print("Mrm resource-ID helpers validate inputs and allocations")
