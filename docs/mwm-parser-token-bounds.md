# MWM parser token boundaries

MWM now copies parsed configuration tokens through one bounded helper instead
of narrowing pointer differences to `int`.  Context, modifier, event, and key
symbol tokens remain terminated within their destination arrays.  Parsed menu
function indices are validated before table lookup, and a `KeySym` that cannot
fit the public unsigned-detail value is rejected rather than truncated.

The lowercase helper also passes bytes to the C character-classification APIs
as `unsigned char`, which avoids undefined behavior for non-ASCII locale
bytes.

Icon-box subcontext masks are also complemented in the unsigned `Context`
domain, so removing a subcontext does not pass a signed complement through the
unsigned mask.

The focused source contract is covered by
`tests/automated/check-mwm-parser-token-bounds.py`.
