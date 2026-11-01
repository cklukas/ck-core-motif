---
nav_exclude: true
---
# XmString byte-stream length validation

`XmStringByteStreamLength` checks the three-byte compound-string ASN.1 identifier
before reading length bytes. Null input and a mismatched identifier return zero.
The API has no buffer-length argument; importers must establish that the encoded
header is available before calling it.

The returned size includes the header actually encoded in the input. Previously
the function recomputed the shortest header for the payload length, undercounting
long-form headers with payloads of 0–127 bytes by two. Property importers could
then advance to the wrong position or accept an outer length check that was too
small. Canonical short and long encodings retain their previous sizes; encoded
long headers for small payloads now return the correct size.

Component readers use the same rule. An external compound string may contain a
long-form component length field even when that component's payload is at most
127 bytes. Import, optimization, debug inspection, and truncation now advance
over the header actually present in the stream (two bytes for a short component
header or four bytes for a long one), rather than deriving its width from the
decoded payload length. This preserves canonical streams and valid
non-canonical long-form streams without shifting component payloads.

The regression check is
`tests/automated/check-xmstring-byte-stream-length.py`.

The executable production-function regression covers 266 cases: both outer-header
forms for all lengths 0–127, long-form lengths 128/255/256/65535, short and long
component headers for 0/127-byte payloads, invalid identifier and null input. It
also verifies process-lock balance under ASan/UBSan and records
source hashes, commands, timings and results in `receipt.json`:

```sh
python3 tests/automated/run-xmstring-stream-length-unit.py --out /absolute/fresh/results
```
