# Validate ICC drop-site stream bounds

The ICC drag receiver now rejects a truncated or malformed drop-site stream
before it writes region boxes.  The stream header and animation payload must
be fully present, the declared region count must fit in the remaining data
bytes, and region allocation failure is handled without dereferencing a null
region.  Rejected
streams release their temporary property buffer and receiver state and return
failure to the drop-site manager.

Valid Motif-produced streams retain their existing decoding and geometry
behavior.  The focused source regression is
`tests/automated/check-drag-ds-stream-bounds.py`.
