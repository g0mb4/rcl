#include "str_buf.h"

str_buf_t * str_buf_create(void){
	str_buf_t * buf = (str_buf_t *) malloc(sizeof(str_buf_t));
	if(buf == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	buf->str = (char *) malloc(sizeof(char) * 10);
	if(buf->str == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(buf->str, 0, 10);
	buf->len = 0;
	buf->cap = 10;

	return buf;
}

void str_buf_destroy(str_buf_t * sb){
	if(sb){
		if(sb->str){
			free(sb->str);
		}

		free(sb);
	}
}

void str_buf_add(str_buf_t * sb, const char * s){
	uint32_t len = strlen(s);

	if(sb->len + len >= sb->cap){
		sb->str = realloc(sb->str, sizeof(char) * (sb->len + len + 1));

		if(sb->str == NULL){
			fprintf(stderr, "%s:%d:%s() : realloc() failed.\n", __FILE__, __LINE__, __func__);
			return;
		}

		sb->cap = sb->len + len + 1;
	}

	strcpy((sb->str + sb->len), s);
	sb->len += len;
}
