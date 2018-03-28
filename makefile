all:
	@echo "\nCompiling files, please wait..."
	@echo "________________________"
	@$(MAKE) hamm_seq hamm_omp_a hamm_omp_b hamm_omp_c hamm_pt_a hamm_pt_b hamm_pt_c
	@echo "________________________"
	@echo "\nExecuting all cases..."
	@./hamm_seq $(M) $(N) $(L)
	@./hamm_omp_a $(M) $(N) $(L) $(T)
	@./hamm_omp_b $(M) $(N) $(L) $(T)
	@./hamm_omp_c $(M) $(N) $(L) $(T)
	@./hamm_pt_a $(M) $(N) $(L) $(T)
	@./hamm_pt_b $(M) $(N) $(L) $(T)
	@./hamm_pt_c $(M) $(N) $(L) $(T)

omp:
	@$(MAKE) hamm_omp_a hamm_omp_b hamm_omp_c
	@./hamm_omp_a $(M) $(N) $(L) $(T)
	@./hamm_omp_b $(M) $(N) $(L) $(T)
	@./hamm_omp_c $(M) $(N) $(L) $(T)

posix:
	@$(MAKE) hamm_pt_a hamm_pt_b hamm_pt_c
	@./hamm_pt_a $(M) $(N) $(L) $(T)
	@./hamm_pt_b $(M) $(N) $(L) $(T)
	@./hamm_pt_c $(M) $(N) $(L) $(T)
		
hamm_seq : hamming.o
	@gcc -o hamm_seq hamming.c

hamming.o : hamming.c
	@gcc -c -o hamming.o hamming.c

hamm_omp_a : hamm_omp_a.o
	@gcc -o hamm_omp_a hamming_omp_a.c -fopenmp

hamm_omp_a.o : hamming_omp_a.c
	@gcc -c -o hamm_omp_a.o hamming_omp_a.c -fopenmp

hamm_omp_b : hamm_omp_b.o
	@gcc -o hamm_omp_b hamming_omp_b.c -fopenmp

hamm_omp_b.o : hamming_omp_b.c
	@gcc -c -o hamm_omp_b.o hamming_omp_b.c -fopenmp

hamm_omp_c : hamm_omp_c.o
	@gcc -o hamm_omp_c hamming_omp_c.c -fopenmp

hamm_omp_c.o : hamming_omp_c.c
	@gcc -c -o hamm_omp_c.o hamming_omp_c.c -fopenmp

hamm_pt_a : hamm_pt_a.o
	@gcc -o hamm_pt_a hamming_posix_a.c -lpthread

hamm_pt_a.o : hamming_posix_a.c
	@gcc -c -o hamm_pt_a.o hamming_posix_a.c -lpthread

hamm_pt_b : hamm_pt_b.o
	@gcc -o hamm_pt_b hamming_posix_b.c -lpthread

hamm_pt_b.o : hamming_posix_b.c
	@gcc -c -o hamm_pt_b.o hamming_posix_b.c -lpthread

hamm_pt_c : hamm_pt_c.o
	@gcc -o hamm_pt_c hamming_posix_c.c -lpthread

hamm_pt_c.o : hamming_posix_c.c
	@gcc -c -o hamm_pt_c.o hamming_posix_c.c -lpthread

clean:
	rm -f hamm_seq hamm_omp_a hamm_omp_b hamm_omp_c hamm_pt_a hamm_pt_b hamm_pt_c
	rm -f hamming.o hamm_omp_a.o hamm_omp_b.o hamm_omp_c.o hamm_pt_a.o hamm_pt_b.o hamm_pt_c.o