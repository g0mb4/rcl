#include "rcl.h"

rcle_proc_list_t * rcle_proc_list = NULL;

void rcle_print_attrs(void){
	uint32_t i;
	for(i = 0; ; i++){
		const rcl_attr_desc_t * desc = &rcl_attr_fcns[i];	// to solve warning (Initialization discards qualifiers from pointer target type)

		/* end of array */
		if(desc->attr == NULL){
			break;	// break from the for loop
		}

		printf("%10s :", desc->attr);
		printf(" %s", desc->fcn_get   ? "  get" : " - ");
		printf(" %s", desc->fcn_set   ? "  set" : " - ");
		printf(" %s", desc->fcn_reset ? "reset" : "  -  ");
		printf(" %s", desc->fcn_start ? "start" : "  -  ");
		printf(" %s", desc->fcn_stop  ? " stop" : "  -  ");
		printf("\n");
	}
}

rcle_process_t * create_process(void (* fcn)(rcl_type_t * out, rcl_type_t * in, rcle_process_t * self), rcl_type_t * out, rcl_type_t * in){
	rcle_process_t * ret = (rcle_process_t *)malloc(sizeof(rcle_process_t));
	if(ret == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	ret->state = RS_RUNNING;
	ret->fcn = fcn;
	ret->in = rcl_null();
	ret->out = rcl_null();

	ret->self = (rcle_process_t *)malloc(sizeof(rcle_process_t));
	if(ret->self == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}
	ret->self = ret;

	rcl_copy(ret->in, in);
	rcl_copy(ret->out, out);

	return ret;
}

int rcle_init(void){
	rcle_proc_list = (rcle_proc_list_t *)malloc(sizeof(rcle_proc_list_t));
	if(rcle_proc_list == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return -1;
	}

	memset(rcle_proc_list, 0, sizeof(rcle_proc_list_t));
	rcle_proc_list->elements = (rcle_process_t **)malloc(sizeof(rcle_process_t *) * RCLE_PROC_LIST_INIT_SIZE);
	if(rcle_proc_list->elements == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return -2;
	}

	memset(rcle_proc_list->elements, 0, sizeof(rcle_process_t *) * RCLE_PROC_LIST_INIT_SIZE);
	rcle_proc_list->cap = RCL_LIST_INIT_SIZE;

	return 0;
}

void rcle_destroy(void){
	uint32_t i;
	if(rcle_proc_list){
		if(rcle_proc_list->elements){
			for(i = 0; i < rcle_proc_list->cap; i++){
				if(rcle_proc_list->elements[i]){
					free(rcle_proc_list->elements[i]);
				}
			}
			free(rcle_proc_list->elements);
		}
		free(rcle_proc_list);
	}
}

int rcle_add_fcn(rcle_process_t * proc){
	if(proc == NULL){
		return -1;
	}

	uint32_t i;

#ifdef _DEBUG
{
	str_buf_t * sb = str_buf_create();
	rcl_val_to_str_buf(sb, proc->in);
	printf(" ADD: %s\n", sb->str);
	str_buf_destroy(sb);
}
#endif

	/* search for an empty space */
	for(i = 0; i < rcle_proc_list->cap; i++){
		if(rcle_proc_list->elements[i] == NULL){
			rcle_proc_list->elements[i] = proc;
			return 0;
		}
	}

	/* no free space */
	rcle_proc_list->elements = realloc(rcle_proc_list->elements, sizeof(rcl_type_t *) * rcle_proc_list->cap * 2);
	if(rcle_proc_list->elements == NULL){
		fprintf(stderr, "%s:%d:%s() : realloc() failed.\n", __FILE__, __LINE__, __func__);
		return -2;
	}

	rcle_proc_list->cap *= 2;
	rcle_proc_list->elements[i] = proc;

	return 0;
}

void rcle_remove_next_fcn(rcl_type_t * out){
	uint32_t i;
	for(i = 0; i < rcle_proc_list->cap; i++){
		if(rcle_proc_list->elements[i] && rcle_proc_list->elements[i]->state == RS_DONE){

			rcl_copy(out, rcle_proc_list->elements[i]->out);

			free(rcle_proc_list->elements[i]);
			rcle_proc_list->elements[i] = NULL;
			return;
		}
	}
}

void rcle_main_step(rcl_type_t * out){
	rcl_copy(out, rcl_null());

	uint32_t i;
	/* run loop */
	for(i = 0; i < rcle_proc_list->cap; i++){
		if(rcle_proc_list->elements[i] && rcle_proc_list->elements[i]->state == RS_RUNNING){
			rcle_proc_list->elements[i]->fcn(rcle_proc_list->elements[i]->out, rcle_proc_list->elements[i]->in, rcle_proc_list->elements[i]->self);
		}
	}

	rcle_remove_next_fcn(out);

#ifdef _DEBUG
{
	str_buf_t * sb = str_buf_create();
	rcl_val_to_str_buf(sb, out);
	printf(" OUT: %s\n", sb->str);
	str_buf_destroy(sb);
}
#endif
}
