# Compound-text direction-stack allocation

Public and resource-converter compound-text contexts now check their context
and initial direction-stack allocations. Partial state is released and the
conversion reports failure instead of dereferencing invalid storage.

The regression check is
`tests/automated/check-resencod-context-allocation.py`.
