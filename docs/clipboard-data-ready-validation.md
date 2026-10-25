# Clipboard data-ready event validation

`ClipboardDataIsReady` now rejects missing event/private state and validates
the retrieved format record before reading its cut-by-name flag. Invalid
records are released and reported as not ready instead of invoking fatal
corruption handling or dereferencing malformed storage.
