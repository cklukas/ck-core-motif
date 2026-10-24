# Mrm write-open recovery

When creating a write-mode database, Mrm first creates and closes the file,
then reopens it for read/write access. If that reopen fails, the recovery path
now unlinks the partial file before returning failure, avoiding a misleading
empty database artifact.
