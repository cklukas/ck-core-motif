# Accessibility metadata bounds

The public accessibility snapshot stores table dimensions and text positions
as signed `int` values because that is the width used by the AT-SPI protocol
methods.  Motif's DataGrid counts and text APIs can use wider unsigned
quantities.  The snapshot query therefore saturates values above `INT_MAX`
instead of narrowing them directly and turning them negative.

This keeps large or malformed widgets from publishing negative row counts,
columns, text lengths, or caret offsets.  Normal-sized widgets retain their
exact values and the change does not alter the public ABI.
