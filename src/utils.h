
#include "carbon_conf.h"

#define ERROR_LINE_SIZE 200

/* 	return 0 if success, text_p has result 
	don't forget to free(*text_p)
*/
int utils_read_file(char** text_p, const char* file_path); 

// string util
bool utils_char_in_str(char c, const char* string);
int  utils_char_count_in_str(char c, const char* string);
int  utils_pos_to_line(size_t pos, const char* src, char* buffer, char* location_str, size_t error_len); // used by escape the warning 
void utils_print_char(char c, bool new_line); // prints '\n' as "\\n"
void utils_print_str_without_esc(const char* str, bool new_line, bool print_dquote);

// dipricated
void utils_error_exit(const char* err_msg, size_t pos, const char* src, const char* file_name);
void utils_warning_print(const char* warn_msg, size_t pos, const char* src, const char* file_name, size_t err_len);
// free_msg -> free the error message after writing to new error obj
struct CarbonError* utils_make_error(char* err_msg, enum ErrorType err_type, size_t pos, const char* src, const char* file_name, bool free_msg, size_t error_len);