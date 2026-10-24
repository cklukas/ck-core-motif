# Mrm record-buffer input validation

Mrm buffer acquisition, record lookup, and record initialization now reject null
file and output-buffer pointers before touching the shared buffer pool. Valid
read, write, and initialization behavior is unchanged; malformed calls return
`MrmFAILURE` instead of dereferencing invalid context.

The source-level regression is `tests/automated/check-mrm-buffer-input.py`.
