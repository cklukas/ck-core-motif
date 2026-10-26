# TextField modify-verify input buffers

TextField modify-verify setup now validates insertion length and character
width before constructing its byte callback block.  The size is bounded to
both the signed block-length field and Xt's allocation type, and allocation
failure returns before conversion or callback delivery.

The focused source contract is checked by
`tests/automated/check-textfield-modify-verify-allocation.py`.
