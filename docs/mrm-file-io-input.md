# Mrm low-level file I/O validation

Mrm close, read-block, and write-block helpers now reject null descriptors or
buffers before system calls. Read and write operations also reject nonpositive
record numbers, preventing seeks before the UID file. Valid file I/O retains
its existing behavior and return codes.

The source-level regression is `tests/automated/check-mrm-file-io-input.py`.
