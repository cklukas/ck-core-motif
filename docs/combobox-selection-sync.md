# ComboBox selection-to-text synchronization

When a child List selection changes, ComboBox mirrors the selected item into
its edit field.  The old path assumed that positive counts implied non-null
arrays and that the first position was within the item count.  Synchronization
now requires both arrays and a one-based position in range before indexing the
item table; malformed or failed List queries leave the edit field unchanged.

The regression contract is checked by
`tests/automated/check-combobox-selection-sync.py`.
