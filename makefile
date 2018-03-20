CC=gcc


humm: humming.c
    $(CC) -o humm humming.c 
omp : humming_omp.c
	$(CC) -o humm_omp humming_omp.c

clean : rm humm