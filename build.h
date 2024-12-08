
typedef struct {
 struct list_head node;
 GLchar *name;
 GLint address;
} symbol_t;

typedef struct {
 GLuint frag,vect, prog;
 unsigned char *frag_buf;
 int frag_len;


 GLchar *vect_buf;
 GLint vect_len;

 symbol_t *symbols;

} prog_t;


GLint get_address(prog_t *in, GLchar *name);
prog_t *build(unsigned char *frag_name, unsigned char *vect_name, 
	     int symbol_count, ...);
