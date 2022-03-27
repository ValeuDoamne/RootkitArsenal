#include <Windows.h>
#include <stdio.h>

int main()
{
	BOOL ok;
	DWORD SectorsPerCluster = 0;
	DWORD BytesPerSector    = 0;
	DWORD NumberOfFreeCluseters = 0;
	DWORD TotalNumberOfClusters = 0;

	ok = GetDiskFreeSpace(
		0,
		&SectorsPerCluster,
		&BytesPerSector,
		&NumberOfFreeCluseters,
		&TotalNumberOfClusters
	);

	printf("SectorsPerCluster: %d\n", SectorsPerCluster);
	printf("BytesPerSector: %d\n", BytesPerSector);
	printf("NumberOfFreeClusters: %d\n", NumberOfFreeCluseters);
	printf("TotalNumberofClusters: %d\n", TotalNumberOfClusters);

	getchar();
	return 0;
}
