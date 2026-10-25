# Clipboard header close size guard

`ClipboardClose` now ignores null headers and rejects `currItems` values whose
serialized header size would overflow `unsigned long`. This prevents malformed
clipboard state from wrapping the property length or dereferencing a null
header during cleanup.
