# Clipboard delete-state validation

`ClipboardIsMarkedForDelete` now validates the item ID, lookup status, and
data-item record length before reading `deletePendingFlag`. Invalid items are
released and treated as unmarked, allowing deletion scans to fail closed
without fatal corruption handling.
