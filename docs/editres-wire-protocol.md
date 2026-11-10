# Editres protocol handler

Any client of the X display can send a Motif application the Editres client
message and then serve it a command through a selection, so everything in
`lib/Xm/EditresCom.c` treats the request as untrusted input. The file owns the
whole protocol: request parser, reply writer and command handlers. It no
longer calls the `_XEditRes*` helpers of libXmu, which take the length of a
request from the request itself and read one byte past the data; only the
protocol definitions are kept, as private constants.

## Request parsing

- A request is parsed with a reader that is bound to the bytes the toolkit
  delivered. The length field of the request header must fit in those bytes,
  and no field may extend past it; a request that does not fit is answered
  with a failure reply. An 8, 16 or 32 bit read at the end of the data fails
  instead of returning the next byte.
- Counts taken from the stream (entries, widget path length, string length)
  are checked against the bytes that are left before anything is allocated. A
  path must name at least the root of the widget tree. Every entry of a
  request is released when the request is, including requests that fail
  half way.
- Signed 16 bit values (the FindChild coordinates) are decoded in two's
  complement. Negative values used to decode as zero.
- The selection value handed to the callback by the toolkit is released on
  every path, so a stream of requests cannot exhaust memory through the
  request buffers.

## Finding widgets

A widget is named on the wire by the path from the root of the tree, each
element the low 32 bits of the widget address. A path is resolved by walking
the live widget tree, comparing those 32 bits, so that no number from the wire
becomes a pointer and a path with an element that is not in the tree is
reported as a widget that no longer exists. This also makes the protocol work
when widget addresses do not fit in 32 bits, which the earlier code did not
handle.

## Replies

- The reply is built in a stream that grows up to 32 MiB. A reply that would
  be larger, or cannot be allocated, is replaced by a failure message. Counts
  that are 16 bit fields on the wire (entries, errors, resources, widgets in
  the tree, string lengths) saturate at 65535 and the bytes that follow match
  the count that was sent.
- Each conversion of the Editres selection hands the toolkit its own copy of
  the reply, so a command that arrives while an earlier reply is still being
  transferred cannot change or free that value.

## Client messages

The message names the selection to fetch the command from. It is only used
when it is the `EditresComm` selection, which the Editres client always uses.
A message that names `PRIMARY`, `CLIPBOARD` or any other selection is ignored,
so it cannot make the application fetch a command from, or take ownership of,
that selection. The protocol atoms are looked up for the display the message
arrived on, and a failed lookup ends the processing of the message.

## SetValues

The protocol carries every value as a string. A request whose value type is not
`String` is refused. The value is converted by the toolkit like any resource
database value, and conversion messages are reported with the offending string.
Toolkit warning messages are expanded with the parameters the toolkit supplies
(`%s` only) instead of being used as printf formats.

A widget can keep the pointer it is given for a `String` resource. The value of
such a resource is therefore copied, and the copy lives as long as the process;
the total of these copies is limited to 1 MiB, after which the entry is
reported as an error.

The `editresBlock` resource (`none`, `setvalues`, `all`) is unchanged.

## Validation

`tests/automated/run-editres-wire-protocol.py` compiles the production file
into `tests/automated/check-editres-wire-protocol.c` with AddressSanitizer and
UndefinedBehaviorSanitizer (including leak detection) and runs it on a virtual
X display. The program builds requests byte by byte from the protocol
description and checks, among others: widget tree, resource, geometry, value
and SetValues replies; FindChild with negative coordinates; requests cut
inside a field, with a length that exceeds the data, with fields that exceed
the length, with impossible counts and with an empty or forged widget path;
retained `String` values; blocking; the reply limit; and, through a second X
connection acting as the resource editor, the complete exchange and the
refusal to use a selection other than `EditresComm`.

`--source-dir` and `--control` run the same program against an earlier copy of
`EditresCom.c`; the earlier code fails it.
