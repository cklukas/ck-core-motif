# Clipboard mark-item validation

`ClipboardMarkItem` now validates data-item lookup status and record length
before setting `deletePendingFlag`. Missing, failed, or undersized records are
released and ignored instead of entering fatal corruption handling or being
dereferenced as a full structure.
