# TextField transfer length conversions

TextField selection insertion already rejects transfer payloads that cannot be
represented by its `int` editing API or by Xt's `Cardinal` allocator. The
conversion path now expresses those proven bounds explicitly: character counts
use `int`, byte allocations use checked `size_t` arithmetic cast to `Cardinal`,
and multibyte conversion limits use `size_t`.

This preserves the existing rejection behavior for oversized payloads while
avoiding implementation-defined unsigned-to-int narrowing on 64-bit builds.
The contract is covered by
`tests/automated/check-textfield-transfer-length-casts.py`.
