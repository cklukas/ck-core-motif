Release rejected drop-transfer values
=====================================

Malformed drop-transfer selection callbacks now release any non-null Xt-owned
selection buffer before failing the transfer.  This covers missing callback
state, typeless/null-payload/invalid-format results, and exhausted transfer
lists.  Values delivered to the application callback retain the existing
ownership contract.

The focused regression is:

```text
python3 tests/automated/check-drop-transfer-rejected-value-cleanup.py
```
