# Mrm hash-table caller failure handling

Widget-class and resource-name registration now handle hash-entry allocation
failure explicitly. Class registration removes its just-created descriptor
before returning, while resource-name registration rejects invalid arrays and
stops on a failed insertion instead of dereferencing a null entry. Successful
registration behavior is unchanged.

The source-level regression is
`tests/automated/check-mrm-wci-hash-callers.py`.
