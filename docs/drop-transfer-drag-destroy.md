# Drop-transfer drag destruction

Drop-transfer timers and processing can outlive the drag context's destruction
callback.  They now check the cleared drag handle and receiver information
before reading ICC properties or dereferencing the drag context.  Invalidated
state is finalized as a failed transfer, releasing temporary arrays and
destroying the drop-transfer object safely.

Regression coverage: `tests/automated/check-drop-transfer-drag-destroy.py`.
