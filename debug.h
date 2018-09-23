#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "rcl.h"

#ifdef _DEBUG
	#define dbg_printf( str ) 	fprintf(stderr, "%s:%d:%s() : %s\n", __FILE__, __LINE__, __func__, str)
#else
	#define dbg_printf( str ) 	fprintf(stderr, "%s\n", str)
#endif

#ifdef _DEBUG
	#define dbg_val( val )	\
		{	\
			str_buf_t * sb = str_buf_create(); \
			rcl_val_to_str_buf(sb, val ); \
			printf(" %s:%d:%s() : val: %s\n", __FILE__, __LINE__, __func__, sb->str); \
			str_buf_destroy(sb); \
		}
#else
	#define dbg_val( val ) 	{}
#endif

#endif
