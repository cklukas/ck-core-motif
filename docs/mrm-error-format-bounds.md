---
nav_exclude: true
---
# Mrm error formatting bounds

Mrm error reporting uses fixed 300-byte buffers for the latest diagnostic and
for hierarchy-open messages. Those buffers now use `snprintf`, so long module,
resource, or filename text is truncated safely instead of overwriting stack
storage. Normal short diagnostics retain their existing text and status.

The source-level regression is
`tests/automated/check-mrm-error-format-bounds.py`.
