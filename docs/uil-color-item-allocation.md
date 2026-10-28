# UIL color-item allocation failure handling

`sar_make_color_item` now checks its temporary color-item node before saving
source metadata or filling fields.  An allocation failure selects the existing
null-frame error path without dereferencing a null node.  If later validation
rejects the color item, the temporary node is released before returning, so
the error path does not leak the partially built item.

The source-contract regression is
`tests/automated/check-uil-color-item-allocation.py`.
