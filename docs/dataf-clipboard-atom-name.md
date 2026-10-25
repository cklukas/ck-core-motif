# Clipboard atom-name failure

The text-field clipboard path now handles a failed `XGetAtomName` lookup
before calling `XmClipboardCopy`.  It cancels the pending copy and releases
the selected text, converted property data, and label.  Successful clipboard
copies retain the existing atom-name and transfer behavior.
