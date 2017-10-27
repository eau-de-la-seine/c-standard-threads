# C11 THREAD LIBRARY


Cross platform C11 native threads library implementation for Unix and Windows environments


Implemented standard functions:  

	thrd_create
	thrd_equal
	thrd_current
	thrd_yield
	thrd_exit
	thrd_detach
	thrd_join
	mtx_destroy
	mtx_init		/* Partially: mtx_timed option not implemented yet */
	mtx_lock
	mtx_trylock
	mtx_unlock
	
Working in progress functions:  

	thrd_sleep
	mtx_timedlock

Under Linux, use:
	gcc -pthread -o threads.h threads.c main.c
