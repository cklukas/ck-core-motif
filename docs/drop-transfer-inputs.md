# Drop-transfer input contract

`XmDropTransferStart` now returns `NULL` when its reference widget is absent,
before display or application-context lookup.  `XmDropTransferAdd` now
returns without dispatch when its widget is absent, when the transfer count is
zero, or when the transfer array is missing.  Valid drop-transfer construction
and additions are unchanged.
