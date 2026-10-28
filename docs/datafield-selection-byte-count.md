# DataField selection byte counting

The wide-character selection path counts the multibyte bytes needed before it
allocates a transfer buffer.  The counter now treats a failed `wctomb`
conversion as an empty result and checks each addition for `int` overflow.
This prevents invalid or wrapped lengths from reaching selection allocation
and conversion callbacks while preserving the existing successful path.
