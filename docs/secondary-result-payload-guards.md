Reject malformed secondary-selection results
=============================================

`SecondaryDone` now fails closed when the selection owner reports an invalid
format or a non-empty result without both a type atom and payload pointer.
Rejected payloads are released before the secondary selection is disowned.
The valid empty cancellation result and 8/16/32-bit typed results retain their
existing behavior.

The focused regression is:

```text
python3 tests/automated/check-secondary-result-payload.py
```
