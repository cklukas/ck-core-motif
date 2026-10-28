# Text wide conversion overflow result

`_XmTextBytesToCharacters` already reports an invalid multibyte sequence as
`-1`. When a valid conversion produced more characters than its legacy `int`
result can represent, it previously returned `0`, making an oversized value
look like an empty conversion. The overflow path now returns `-1` as well, so
callers can take the existing conversion-failure path instead of silently
dropping text.

The source contract is covered by
`tests/automated/check-text-wide-conversion-overflow-result.py`.
