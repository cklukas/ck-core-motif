# Picture state allocation

Picture parsing and state construction now treat allocation as a transaction.
`XmParsePicture` checks its picture, source-copy, and initial node-table
allocations before entering the parser, releasing earlier allocations on
failure.  `XmGetNewPictureState` now treats state construction as a
transaction.  It
rejects a null picture and node counts that cannot be represented by the
ABI-preserved `int statesize` field, and checks every allocation before the
state is initialized or published.  If a later allocation fails, all earlier
allocations are released and the function returns `NULL`.

The state-vector size is computed in `size_t` before conversion to `int`, so a
large or malformed picture cannot overflow the old `num_nodes * sizeof(char)`
expression.  Existing callers that receive a valid state retain the same
layout and behavior; callers must continue to handle the documented `NULL`
result from allocation failure.  The public processing, deletion, query, and
auto-fill helpers also reject null handles, making cleanup and optional
processing paths safe without changing valid-handle behavior.  Auto-fill also
stops and returns the current partial string when character processing cannot
grow its buffer, instead of retrying indefinitely.

The source-level regression check is
`tests/automated/check-picture-state-allocation.py`.
