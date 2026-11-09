# Mrm legacy font-list fixup failure

Legacy font-list replacement now frees its temporary replacement and returns the
fixup error before replacing the resource-context buffer. The same ownership
rule applies to widget resource updates and callback literal conversion.

The focused regression is `tests/automated/check-mrm-fontlist-allocation.py`.
