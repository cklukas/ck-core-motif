# Color-name length bounds

Color database names are copied into fixed-size selector records.  Names at or
above the record limit are truncated, and the loop length is reduced to the
truncated size before normalization.  This prevents an overlong source name
from writing past either fixed destination buffer.
