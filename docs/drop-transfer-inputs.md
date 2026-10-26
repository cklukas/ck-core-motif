# Drop-transfer input contract

`XmDropTransferStart` now returns `NULL` when its reference widget is absent,
before display or application-context lookup.  `XmDropTransferAdd` now
returns without dispatch when its widget is absent or when a nonzero transfer
count has no transfer array.  Valid drop-transfer construction and additions
are unchanged.
