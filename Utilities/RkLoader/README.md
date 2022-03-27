# Rootkit Loader

This utility will use the Windows API to create a new service and load and executate a new driver into the kernel.

## Usage
```
	RkLoader.exe [NameOfTheService] [FullPathToFile]
```

Example:
```
	RkLoader.exe secret C:\Windows\System32\evil.sys
```
