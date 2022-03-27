# Command & control

This is just an idea of a rootkit(very rudimentary, may I add), the driver being in the kernel space communicating with a user space program providing it with evil deeds.

The driver will create a new device "\\.\\msnetdiag" and the userprogram will access it and communicate with the driver.
