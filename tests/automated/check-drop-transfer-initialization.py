#!/usr/bin/env python3
"""Check that DropTransfer initialization handles copy allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text(encoding="utf-8")
start = source.index("static void \nInitialize(")
end = source.index("static Boolean \nSetValues(", start)
region = source[start:end]

assert "SIZE_MAX / sizeof(XmDropTransferEntryRec)" in region
assert "if (dtp->drop_transfer_lists == NULL)" in region
assert "if (dtp->drop_transfer_lists[0].transfer_list == NULL)" in region
assert "no_drop_transfers:" in region
assert "dtp->num_drop_transfers = 0;" in region
print("PASS DropTransfer initialization allocation failure is guarded")
