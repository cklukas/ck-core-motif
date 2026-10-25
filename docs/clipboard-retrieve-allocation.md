# Clipboard retrieval allocation failure handling

The internal `ClipboardRetrieveItem` helper now checks its output allocation
before copying data. If a non-empty result cannot be allocated, it releases the
property buffer, clears the output pointer and length, and returns
`ClipboardFail`. Empty results remain valid and do not require an allocation.
This protects all clipboard operations that retrieve item records or data.
