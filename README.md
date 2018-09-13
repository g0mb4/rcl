# rcl
Robot Control Language

# why?
I started a project dealing with robots, so I thought it's a perfect opportunity to dust my flex/bison skills. The goal it to create a simple interptreter, that can execute commands coming from a TCP/IP server.

This is an early version of the implmenetation of rcl.

The Makefile is prepared for Win->Linux (RPi) cross-development (except for debugging :( ).

# syntax
```
operator attribute (parameter)
```
*operators are built-in*:
- get
- set
- reset
- start
- stop

*attributes can be added through* ```rcl_fcns.h```:
```
const rcl_attr_desc_t rcl_attr_fcns[] = {
	...
	{ "conn", 	 /* attribute */
	   conn_get, /* pointer to get */
	   NULL,	 /* pointer to set */
	   NULL,	 /* pointer to reset */
	   NULL,	 /* pointer to start */
	   NULL		 /* pointer to stop */
	},
	...
	/* end of structure, do not change! */
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};
```
A function pointer has a signature of ``` int (* fcn)(rcl_type_t *) ``` in every cases , as you can see in ``` rcl.h / struct S_RCL_ATTR_DESC```.

See ```rcl.c/rcl_val_to_string()``` as an example for dealing with the ```rcl_type_t *``` type.

*parameters can be*:
- integers (```int```)
- unsigned integers (```unsigned int```)
- doubles (```double```)
- strings (```char *```)
- lists (parameters between ```[]```-s, yep, it's recursive)

# examples
```
get conn
start move [20.0, 10.0]
set path [[20.0, 10.0], [5, 2], [0, 0]]
set camera "stream"
set my_sensor 0xAA
```
