# Mrm index mutation inputs

Index split, node-space, and parent-link helpers now reject missing record
buffers, missing file handles, and invalid parent or child record numbers
before accessing record headers.

The source-level regression is
`tests/automated/check-mrm-index-mutation-inputs.py`.
