# Clipboard inquiry lock cleanup

Clipboard inquiries now close their temporary header and release the clipboard
lock when an external selection cannot provide data.  This applies to format
count, format-name, and format-length inquiries, allowing subsequent clipboard
operations to retry instead of remaining locked after `ClipboardNoData`.
