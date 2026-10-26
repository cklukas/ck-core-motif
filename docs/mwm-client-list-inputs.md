# MWM client-list inputs

`SetMwmClientList` now rejects the invalid combination of a nonzero client
count and a `NULL` window-list pointer before calling `XChangeProperty`.

Valid nonempty lists continue to update `_MWM_CLIENT_LIST` normally. A zero
count is still passed through so callers can clear the property without
allocating a dummy array.
