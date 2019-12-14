//
#include "utilsFLDR.h"

int read_from_dir(char *input_dir_path) {

    DIR *dir_stream;
    struct dirent *dir_entry;
    int FOUND = 0;
    int NOT_FOUND = 1;
    int READ_ERROR = 5;
    int OPEN_ERROR = 9;
    
    dir_stream = opendir(input_dir_path);
    
    while (dir_stream) {
        errno = 0;
        if ((dir_entry = readdir(dir_stream)) != NULL) {
            printf("%s\n", dir_entry->d_name);
            if (strcmp(dir_entry->d_name, input_dir_path) == 0) {
                closedir(dir_stream);
                return FOUND;
            }
        } else {
            if (errno == 0) {
                closedir(dir_stream);
                return NOT_FOUND;
            }
            closedir(dir_stream);
            return READ_ERROR;
        }
    }
    
    return OPEN_ERROR;
}

int write_to_dir(char * output_dir_path, mode_t permissionMode) {

	DIR *dir_stream;
	mode_t tpermissionMode = 0777; // rrw permission
	dir_stream = opendir(output_dir_path);

	if(dir_stream == NULL) {
		// create new directory if(!dirExist) and write the file
		int mkdirErr;
		mkdirErr = mkdir (output_dir_path, tpermissionMode);
	} 
	else
		return 1;
		// just write the file to an already created directory

return 0;
}



