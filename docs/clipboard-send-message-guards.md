# Clipboard send-message guards

`ClipboardSendMessage` now rejects a missing format record and validates the
clipboard header before writing `recopyId` for a data-request message. Invalid
records are released and the message is not sent; valid cut-by-name requests
remain unchanged.
