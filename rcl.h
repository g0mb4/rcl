/* Robot Control Language */

#ifndef __RCL_H__
#define __RCL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "str_buf.h"

#include "rcl.tab.h"

enum rcl_types { T_NULL = 0, T_NUM, T_BYTE, T_BOOL, T_STR, T_LIST };

#define RCL_LIST_INIT_SIZE	2

#define RCL_FMT_BYTE_TO_BINARY_FMT "%c%c%c%c%c%c%c%c"
#define RCL_FMT_BYTE_TO_BINARY(byte)  \
  		(byte & 0x80 ? '1' : '0'), \
  		(byte & 0x40 ? '1' : '0'), \
  		(byte & 0x20 ? '1' : '0'), \
  		(byte & 0x10 ? '1' : '0'), \
  		(byte & 0x08 ? '1' : '0'), \
  		(byte & 0x04 ? '1' : '0'), \
  		(byte & 0x02 ? '1' : '0'), \
  		(byte & 0x01 ? '1' : '0')

/* struct def because of bison */
struct S_RCL_TYPE {
	uint8_t type;
	double number;
	uint8_t byte;
	bool boolean;
	char * string;
	struct S_RCL_LIST * list;
};

struct S_RCL_LIST {
	uint32_t len;
	uint32_t cap;
	struct S_RCL_TYPE ** elements;
};

typedef struct S_RCL_TYPE rcl_type_t;
typedef struct S_RCL_LIST rcl_list_t;

typedef struct S_RCL_ATTR_DESC {
    const char * attr;
	void (* fcn_get)(rcl_type_t * out, rcl_type_t * in);
	void (* fcn_set)(rcl_type_t * out, rcl_type_t * in);
	void (* fcn_reset)(rcl_type_t * out, rcl_type_t * in);
	void (* fcn_start)(rcl_type_t * out, rcl_type_t * in);
	void (* fcn_stop)(rcl_type_t * out, rcl_type_t * in);
} rcl_attr_desc_t;

extern const rcl_attr_desc_t rcl_attr_fcns[];	// array def in rcl_fcns.c

/* for bison to parse string */
typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse(struct S_RCL_TYPE * parse_param);
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

#define RCL_MSG_SIZE	1024
char rcl_error_msg[RCL_MSG_SIZE];		// 	used in rcl.y

int rcl_parse(struct S_RCL_TYPE * parse_output, const char *s);

struct S_RCL_TYPE * rcl_null(void);
struct S_RCL_TYPE * rcl_number(double n);
struct S_RCL_TYPE * rcl_byte(uint8_t b);
struct S_RCL_TYPE * rcl_boolean(bool b);
struct S_RCL_TYPE * rcl_string(const char * s);
struct S_RCL_TYPE * rcl_list(struct S_RCL_TYPE * value);

void rcl_copy(rcl_type_t * dst, const rcl_type_t * src);
void rcl_free(rcl_type_t * value);

struct S_RCL_TYPE * rcl_list_add(struct S_RCL_TYPE * list, struct S_RCL_TYPE * value);

char * rcl_op_to_string(int op);
void rcl_val_to_string(char * str, const struct S_RCL_TYPE * value);
void rcl_val_to_str_buf(str_buf_t * sb, const struct S_RCL_TYPE * value);

void rcl_execute(rcl_type_t * parse_output, int op, const char * attr, struct S_RCL_TYPE * param);

#endif
