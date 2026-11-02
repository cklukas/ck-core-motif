# Mrm wide-character fixup cleanup

When Mrm converts a legacy wide-character literal, it allocates a temporary
wide-character buffer before calling `mbstowcs`.  Invalid multibyte input makes
that conversion fail without producing a widget value.

The failure path now releases the temporary buffer before returning
`MrmFAILURE`.  Valid wide-character literals and the existing shortened-buffer
reallocation path retain their previous behavior.  Both allocations use the
already preflighted byte count converted explicitly to Xt's `Cardinal` size
contract.

`tests/automated/check-mrm-conversion-allocation.py` guards this cleanup.
