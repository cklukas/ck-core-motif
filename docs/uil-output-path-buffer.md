---
nav_exclude: true
---

# UIL output path storage

The UIL compiler opens the requested UID output file and reports that same
caller-owned path in diagnostics and status callbacks.  It no longer routes
the path through a fixed 256-byte local result buffer.  Long, valid output
paths therefore retain their complete spelling without risking a local-buffer
overwrite or a truncated status filename.

The low-level MRMs resolved-name output is optional for this invocation, so
the compiler passes `NULL` and consistently uses the command's resource-file
path for both diagnostics and `Uil_current_file`.

`tests/automated/check-uil-output-path-buffer.py` verifies that the emitter
does not reintroduce the fixed result buffer.
