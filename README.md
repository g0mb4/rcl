# rcl
Robot Control Language

# why?
I started a project dealing with robots, so I thought it's a perfect opportunity to dust off my flex/bison skills. The goal it to create a simple interptreter, that can execute commands coming from a TCP/IP server.

This is an early version of the implmenetation of rcl.

The Makefile is prepared for Win->Linux (RPi) cross-development.

# comiple
Modify the ```PREFIX``` and the ```SSH_*``` parameters in the Makefile.
for debugging:
```> make debug```
for release:
```> make && make upload```

# syntax
```
operator attribute (parameter)
```
*operators are built-in*:
- ```get```
- ```set```
- ```reset```
- ```start```
- ```stop```

*attributes can be added through* ```rcl_fcns.h```:
```
const rcl_attr_desc_t rcl_attr_fcns[] = {
	...
	{ "conn",               /* attribute */
	   (void *)&conn_data,  /* data of the attribute */	
	   sizeof(conn_data),   /* size of the data */
	   conn_get,            /* pointer to get */
	   NULL,                /* pointer to set */
	   NULL,                /* pointer to reset */
	   conn_start,          /* pointer to start */
	   NULL,                /* pointer to stop */
	},
	...
	/* end of structure, do not change! */
	{ NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL }
};
```
The functions have the signature of ```void fcn(rcl_type_t * /* out */, rcl_type_t * /* in */, rcle_process_t *  /* self */)``` in every case, as you can see in ```rcl.h / struct S_RCL_ATTR_DESC```.
A simple example for the ```get``` and ```start``` operator on the ```conn``` attribute, which has been added to the  ```rcl_attr_fcns[]```, can be seen bellow.
```
typedef struct S_CONN_DATA {
	char * str;
	uint32_t ctr;
} conn_data_t;

static conn_data_t conn_data = { "conn_str", 0 };

void conn_get(rcl_type_t * out, rcl_type_t * in, rcle_process_t * self){
	if(in->type != T_NULL){
		rcl_copy(out, rcl_error("no parameter is needed"));
	} else {
		conn_data_t * data = ((conn_data_t *)self->data);

		rcl_copy(out, rcl_string(data->str));
	}

	RCL_RETURN( self );
}

void conn_start(rcl_type_t * out, rcl_type_t * in, rcle_process_t * self){
	if(in->type != T_LIST){
		rcl_copy(out, rcl_error("list parameter is needed"));
		RCL_RETURN( self );
	}

	conn_data_t * data = ((conn_data_t *)self->data);
	rcl_type_t * element = rcl_list_get(in, data->ctr++);
	if(element->type == T_ERR){
		rcl_copy(out, rcl_null());
		RCL_RETURN( self );
	} else {
		dbg_val( element );
	}
	rcl_free(element);
}

```
See ```rcl.c / rcl_val_to_string()``` as an example for dealing with the ```rcl_type_t *``` type.

*parameters can be*:
- null-s (indication of the NULL value)
- numbers (integers or floating point values)
- bytes (8 bit values, which is an integer starts with ```0x (hex)``` or ```0b (bin)```)
- booleans (```true``` or ```false``` (which is ```true```, ha-ha))
- strings (characters between ```" (double quote)```-s)
- lists (parameters between ```[ ]```-s, separated by  ```, (comma)```-s, yep, it's recursive)

# examples
```
get conn
start move [20.0, 10.0]
set path [[20.0, 10.0], [5, 2], [0, 0]]
set camera "stream"
set my_sensor 0xAA
```
