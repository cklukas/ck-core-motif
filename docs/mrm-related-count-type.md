# Related Mrm argument types

A serialized related code supplies the count resource for a text, string, or
integer vector. A malformed record could attach a related code to another
representation type; the old path then appended an argument whose value had
never been initialized. The reader now leaves the already validated primary
argument in place and skips that invalid related entry, preserving compatibility
for valid vector resources while avoiding undefined argument data.
