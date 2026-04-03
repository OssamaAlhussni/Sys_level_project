#include "monitor.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
	printf("Error: Missing directory argument.\n");
	printf("Usage: %s <directory_path>\n", argv[0]);
	printf("Example: %s home/project\n", argv[0]);
       return 1;
    }

    FileList list;
    init_list(&list);

    scan_directory(argv[1], &list);

    printf("\nTotal txt files: %d\n", list.count);

    for (int i = 0; i < list.count; i++) {
        backup_file(list.items[i].path);
    }

    free_list(&list);

    return 0;
}
