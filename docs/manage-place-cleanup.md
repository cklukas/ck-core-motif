# Client withdrawal placement cleanup

Client withdrawal now validates workspace records, placement storage, and
stored icon indices before clearing placement entries in WSM and non-WSM paths.
Valid cleanup remains unchanged.

`tests/automated/check-manage-place-cleanup.py` protects the contract.
