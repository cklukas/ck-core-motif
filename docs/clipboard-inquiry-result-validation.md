# Clipboard inquiry result validation

Clipboard inquiries now reject inconsistent external-selection results where a
nonzero byte count is paired with a null allocation pointer.  Count, format,
and length inquiries release their header and lock before returning
`ClipboardNoData`, preventing malformed selection data from becoming an atom
array or format string.
