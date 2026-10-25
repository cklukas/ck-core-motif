# Container trait callback validation

Container item and navigator trait callbacks are now validated before private
dispatch. Missing item `getValues`/`setValues` callbacks use the existing
fallback behavior, while malformed navigator traits are skipped during scroll
updates rather than called through null function pointers.

The regression check is
`tests/automated/check-container-trait-callbacks.py`.
