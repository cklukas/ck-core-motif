# UIL 64-bit machine-listing word format

UIL copies each machine-code word into a fixed-width hexadecimal listing field.
On 64-bit hosts those fields now use sixteen zero-padded digits, so short
values cannot leave uninitialized bytes in the copied listing output. The
existing 32-bit eight-digit representation is unchanged.

Regression: `tests/automated/check-uil-machine-word-format.py`.
