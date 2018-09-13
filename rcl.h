/* Robot Control Language */

#ifndef __RCL_H__
#define __RCL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "rcl.tab.h"

enum rcl_types { T_INT = 1, T_DOUBLE, T_UINT, T_STR, T_LIST };

#define RCL_LIST_INIT_SIZE	2

/* struct def because of bison */
struct S_RCL_TYPE {
	uint8_t type;
	int ival;
	double dval;
	uint32_t uival;
	char * sval;
	struct S_RCL_LIST * list;
};

struct S_RCL_LIST {
	uint32_t len;
	uint32_t cap;
	struct S_RCL_TYPE ** elements;
};

typedef struct S_RCL_TYPE rcl_type_t;
typedef struct S_RCL_LIST rcl_list_t;

/* */
typedef struct S_RCL_ATTR_DESC {
	const char * attr;
	int (* fcn_get)(rcl_type_t *);
	int (* fcn_set)(rcl_type_t *);
	int (* fcn_reset)(rcl_type_t *);
	int (* fcn_start)(rcl_type_t *);
	int (* fcn_stop)(rcl_type_t *);
} rcl_attr_desc_t;

extern const rcl_attr_desc_t rcl_attr_fcns[];	// array def in rcl_fcns.c

struct S_RCL_TYPE * rcl_string(const char * s);
struct S_RCL_TYPE * rcl_int(int i);
struct S_RCL_TYPE * rcl_double(double d);
struct S_RCL_TYPE * rcl_uint(uint32_t u);
struct S_RCL_TYPE * rcl_list(struct S_RCL_TYPE * value);

void rcl_free(rcl_type_t * value);

struct S_RCL_TYPE * rcl_list_add(struct S_RCL_TYPE * list, struct S_RCL_TYPE * value);

char * rcl_op_to_string(int op);
void rcl_val_to_string(char * str, struct S_RCL_TYPE * value);

int rcl_execute(int op, const char * attr, struct S_RCL_TYPE * param);

#endif
