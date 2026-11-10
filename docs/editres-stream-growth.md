# Editres protocol-stream growth

Editres serializes widget inspection data into a growable reply stream. The
stream doubles its capacity up to a fixed limit (32 MiB), checks the addition
that decides whether it still fits, and keeps the old block when a
reallocation fails. Once a write does not fit the stream is marked failed,
later writes are dropped, and the reply is replaced by a short failure message.
A large buffer is released when the next command starts. See
[Editres protocol handler](editres-wire-protocol.md).
