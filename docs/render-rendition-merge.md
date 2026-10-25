# Rendition merge defaults

`XmMERGE_OLD` and related render-table merge modes fill only resources that
are unspecified in the incoming rendition.  Underline and strikethrough are
independent resources; an unspecified strikethrough value must therefore be
tested against `XmAS_IS` separately.  The merge helper now preserves that
contract instead of accidentally using the underline state for both fields.
