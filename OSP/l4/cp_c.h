#ifndef __CP_C__
#define __CP_C__

struct parse_data
{
	char** src;
	int src_count;
	int line_count;
	int is_omitted;	
};

int parse_args(int argc, char const* argv[], struct parse_data* p_data);

int print_lines(const char* filepath, const int line_count);

int* str_to_int(const char* number);

void malloc_error(int errnum);

#endif