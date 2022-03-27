# Sync

Sync it will use only a single thread of execution to read/modify a volatile data structure and the rest of the threads will be locked unsing spinlocks.

This could be used for reading the ``EPROCESS`` block.
