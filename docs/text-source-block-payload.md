# Text-source block payloads

Text-source string readers now reject a block that reports a positive length
but has no payload pointer.  This protects both byte copies and
`mbstowcs` conversions from malformed or failing source callbacks while
leaving valid block handling unchanged.

The focused source contract is checked by
`tests/automated/check-text-source-block-payload.py`.
