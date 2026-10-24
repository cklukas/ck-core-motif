# PNG signature validation

The PNG loader now checks that the complete eight-byte signature was read
before calling `png_sig_cmp`.  Truncated or empty inputs therefore return the
existing bad-signature status without parsing uninitialized bytes.

`tests/automated/check-png-signature-read.py` protects this input contract.
