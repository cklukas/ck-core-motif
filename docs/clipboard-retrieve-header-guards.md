# Clipboard retrieval header guards

Clipboard retrieve transaction entry points now check the header returned by
`ClipboardOpen` before reading or updating retrieval state. Start and internal
retrieve failures release their clipboard lock and application mutex; end
retrieve returns `ClipboardFail` without dereferencing a missing header.
