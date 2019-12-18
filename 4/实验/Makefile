hdrs = ipc.h
opts = -g -c
g_src = glue.c ipc.c
g_obj = glue.o ipc.o
t_src = tobacco.c ipc.c
t_obj = tobacco.o ipc.o
pp_src = paper.c ipc.c
pp_obj = paper.o ipc.o
p_src = producer.c ipc.c
p_obj = producer.o ipc.o
all: producer glue tobacco paper
glue: $(g_obj)
	gcc $(g_obj) -o glue
glue.o: $(g_src) $(hdrs)
	gcc $(opts) $(g_src)
tobacco: $(t_obj)
	gcc $(t_obj) -o tobacco
tobacco.o: $(t_src) $(hdrs)
	gcc $(opts) $(t_src)
paper: $(pp_obj)
	gcc $(pp_obj) -o paper
paper.o: $(pp_src) $(hdrs)
	gcc $(opts) $(pp_src)
producer: $(p_obj)
	gcc $(p_obj) -o producer
producer.o: $(p_src) $(hdrs)
	gcc $(opts) $(p_src)
clean:
	rm glue tobacco paper producer *.o