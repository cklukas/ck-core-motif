# Drop-transfer widget input

`XmDropTransferAdd` now rejects widgets that are not instances of
`xmDropTransferObjectClass` before casting or entering the application lock.
The public API remains unchanged for valid drop-transfer objects while
invalid callers fail safely.

Regression coverage: `tests/automated/check-drop-transfer-widget-input.py`.
