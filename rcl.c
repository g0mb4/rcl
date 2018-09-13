#include "rcl.h"

rcl_type_t * rcl_string(const char * s){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_STR;
	var->sval = strdup(s);
	return var;
}

rcl_type_t * rcl_int(int i){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_INT;
	var->ival = i;
	return var;
}

rcl_type_t * rcl_double(double d){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_DOUBLE;
	var->dval = d;
	return var;
}

rcl_type_t * rcl_uint(uint32_t u){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_UINT;
	var->uival = u;
	return var;
}

rcl_type_t * rcl_list(rcl_type_t * value){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_LIST;
	var->list = (rcl_list_t *)malloc(sizeof(rcl_list_t));
	if(var->list == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var->list, 0, sizeof(rcl_list_t));
	var->list->elements = (rcl_type_t **)malloc(sizeof(rcl_type_t *) * RCL_LIST_INIT_SIZE);
	if(var->list->elements == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var->list->elements, 0, sizeof(rcl_type_t *) * RCL_LIST_INIT_SIZE);
	var->list->len = 0;
	var->list->cap = RCL_LIST_INIT_SIZE;
	var->list->elements[var->list->len++] = value;

#ifdef _DEBUG
	char str[1024];
	memset(str, 0, 1024);
	rcl_val_to_string(str, value);
	printf("created [%d, %d] = %s\n", (var->list->len - 1), var->list->cap, str);
#endif

	return var;
}

void rcl_free(rcl_type_t * value){
	uint32_t i;
	if(value == NULL){
		return;
	}

	switch(value->type){
	case T_INT :
	case T_DOUBLE :
	case T_UINT :
		free(value);
		break;
	case T_STR :
		free(value->sval);
		free(value);
		break;
	case T_LIST:
		for(i = 0; i < value->list->len; i++){
			rcl_free(value->list->elements[i]);
		}
		free(value->list->elements);
		free(value->list);
		free(value);
		break;
	}
}

rcl_type_t * rcl_list_add(rcl_type_t * list, rcl_type_t * value){
		if(list->list->len == list->list->cap){
			list->list->elements = realloc(list->list->elements, sizeof(rcl_type_t *) * list->list->cap * 2);

			if(list->list->elements == NULL){
				fprintf(stderr, "%s:%d:%s() : realloc() failed.\n", __FILE__, __LINE__, __func__);
				return NULL;
			}

			list->list->cap *= 2;
		}

		list->list->elements[list->list->len++] = value;

#ifdef _DEBUG
		char str[1024];
		memset(str, 0, 1024);
		rcl_val_to_string(str, value);
		printf("added [%d, %d] = %s\n", (list->list->len - 1), list->list->cap, str);
#endif

	return list;
}

char * rcl_op_to_string(int op){
	switch(op){
	case OP_GET :
		return strdup("GET");
	case OP_SET :
		return strdup("SET");
	case OP_RESET :
		return strdup("RESET");
	case OP_START :
		return strdup("START");
	case OP_STOP:
		return strdup("STOP");
	}

	return strdup("UNKNOWN");
}

void rcl_val_to_string(char * str, rcl_type_t * value){
	uint32_t i;
	char tmp[50];

	if(value == NULL){
		strcat(str, "NULL");
		return;
	}

	switch(value->type){
	case T_INT :
		sprintf(tmp, "%d", value->ival);
		strcat(str, tmp);
		break;
	case T_DOUBLE :
		sprintf(str, "%.2f", value->dval);
		strcat(str, tmp);
		break;
	case T_UINT :
		sprintf(str, "%u (0x%X)", value->uival, value->uival);
		strcat(str, tmp);
		break;
	case T_STR :
		strcat(str, value->sval);
		break;
	case T_LIST:
		strcat(str, "[ ");
		for(i = 0; i < value->list->len; i++){
			rcl_val_to_string(str, value->list->elements[i]);

			if(i < value->list->len - 1){
				strcat(str, ", ");
			}
		}
		strcat(str, " ]");
		break;
	default:
		strcat(str, "UNKNOWN");
	}
}

int rcl_execute(int op, const char * attr, rcl_type_t * param){
#ifdef _DEBUG
	char str[1024];
	memset(str, 0, 1024);
	rcl_val_to_string(str, param);
	printf(" EXEC: %s %s %s\n", rcl_op_to_string(op), attr, str);
#endif

	uint32_t i, ret = -1;
	for(i = 0; ; i++){
		rcl_attr_desc_t * desc = &rcl_attr_fcns[i];

		/* end of array */
		if(desc->attr == NULL){
			printf("not supported attribute: '%s'\n", attr);
			break;
		}

		if(strcmp(attr, desc->attr) == 0){
			switch(op){
			case OP_GET :
				if(desc->fcn_get){
					ret = desc->fcn_get(param);
				} else {
					printf("not supported opearation '%s' on '%s'\n", rcl_op_to_string(op), attr);
				}
				break;
			case OP_SET :
				if(desc->fcn_set){
					ret = desc->fcn_set(param);
				} else {
					printf("not supported opearation '%s' on '%s'\n", rcl_op_to_string(op), attr);
				}
				break;
			case OP_RESET :
				if(desc->fcn_reset){
					ret = desc->fcn_reset(param);
				} else {
					printf("not supported opearation '%s' on '%s'\n", rcl_op_to_string(op), attr);
				}
				break;
			case OP_START :
				if(desc->fcn_start){
					ret = desc->fcn_start(param);
				} else {
					printf("not supported opearation '%s' on '%s'\n", rcl_op_to_string(op), attr);
				}
				break;
			case OP_STOP:
				if(desc->fcn_stop){
					ret = desc->fcn_stop(param);
				} else {
					printf("not supported opearation '%s' on '%s'\n", rcl_op_to_string(op), attr);
				}
				break;
			}
			break;
		}
	}

	rcl_free(param);
	return ret;
}
