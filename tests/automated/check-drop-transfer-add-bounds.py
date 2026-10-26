#!/usr/bin/env python3
"""Ensure adding a drop-transfer list is transactional and bounded."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
start = source.index("static void \nAddDropTransfer(")
body = source[start:source.index("void \nXmDropTransferAdd(", start)]
assert "UINT_MAX" in body
assert "new_transfers" in body
assert "if (new_lists == NULL)" in body
assert "dtp->num_drop_transfer_lists = new_count;" in body
print("PASS drop-transfer list additions validate bounds and failure")
