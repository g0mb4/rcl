# rcl
Robot Control Language

# why?
I started a project dealing with robots, so I thought it's a perfect opportunity to dust off my flex/bison skills. The goal it to create a simple interptreter, that can execute commands coming from a TCP/IP server.

This is an early version of the implmenetation of rcl.

The Makefile is prepared for Win->Linux (RPi) cross-development (except for debugging :( ).

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
	{ "conn", 	 /* attribute */
	   conn_get,	/* pointer to get */
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
The functions have the signature of ```void fcn(rcl_type_t * /* out */, rcl_type_t * /* in */ )``` in every case, as you can see in ```rcl.h / struct S_RCL_ATTR_DESC```.
A simple example for the ```get``` operator on the ```conn``` attribute, which has been added to the  ```rcl_attr_fcns[]```, can be seen bellow.
```
void conn_get(rcl_type_t * out, rcl_type_t * in){
	/* no parameter is needed for this example */
	if(in != NULL){
		rcl_copy(out, rcl_null());
	}

	rcl_copy(out, rcl_string("works!"));
}
```
See ```rcl.c / rcl_val_to_string()``` as an example for dealing with the ```rcl_type_t *``` type.

*parameters can be*:
- null-s (indication of the NULL value)
- integers
- unsigned integers (integer starts with ```0x (hex)``` or ```0b (bin)```)
- doubles
- booleans (```true``` or ```false``` (which is ```true```, hehe))
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
