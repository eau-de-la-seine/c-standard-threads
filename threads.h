#ifndef C11_THREADS_HEADER
#define C11_THREADS_HEADER

#ifdef __unix__
	#include <pthread.h>
	typedef pthread_t thrd_t;
	typedef pthread_mutex_t mtx_t;
	typedef void*(*POSIX_START_ROUTINE)(void*);
#endif /* __unix__ */
#ifdef _WIN32
	#include <windows.h>
	typedef HANDLE thrd_t;
	typedef HANDLE mtx_t;
#endif /* _WIN32 */



/* Common constants */
#define __OUT__
#define __INOUT__



/**
 * The type thrd_start_t is a typedef of int (*)(void*), which differs from the POSIX equivalent void* (*)(void*)
 * Windows equivalent is LPTHREAD_START_ROUTINE : https://msdn.microsoft.com/en-US/library/aa964928(v=vs.110).aspx
 */
typedef int (*thrd_start_t)(void*);



/**
 * Thread enum
 *
 * thrd_success			indicates successful return value
 * thrd_nomem 			indicates unsuccessful return value due to out of memory condition 
 * thrd_timedout		indicates timed out return value
 * thrd_busy 			indicates unsuccessful return value due to resource temporary unavailable
 * thrd_error 			indicates unsuccessful return value 
 */
enum {
	thrd_success,
	thrd_nomem,
	thrd_timedout,
	thrd_busy,
	thrd_error
};



/**
 * Mutex enum
 *
 * 4 combinations:
 * mtx_plain						a simple, non-recursive mutex is created. 
 * mtx_timed						a non-recursive mutex, that supports timeout, is created.
 * mtx_plain | mtx_recursive		a recursive mutex is created.
 * mtx_timed | mtx_recursive		a recursive mutex, that supports timeout, is created.
 */
enum {
    mtx_plain,
    mtx_recursive,
    mtx_timed
};



/**
 * Creates a new thread executing the function func. The function is invoked as func(arg).
 * If successful, the object pointed to by thr is set to the identifier of the new thread.
 * The completion of this function synchronizes-with the beginning of the thread. 
 *
 * @param thr			pointer to memory location to put the identifier of the new thread
 * @param func			function to execute
 * @param arg			argument to pass to the function
 * @return				thrd_success if the creation of the new thread was successful. Otherwise returns thrd_nomem if there was insufficient amount of memory or thrd_error if another error occurred.	
 */
int thrd_create(__OUT__ thrd_t* thr, thrd_start_t func, void* arg);



/**
 * Returns the identifier of the calling thread. 
 *
 * @return				Returns the identifier of the calling thread.
 */
thrd_t thrd_current(void);



/**
 * Detaches the thread identified by thr from the current environment. The resources held by the thread will be freed automatically once the thread exits.
 *
 * @param thr			identifier of the thread to detach
 * @return				thrd_success if successful, thrd_error otherwise. 
 */
int thrd_detach(thrd_t thr);



/**
 * Checks whether lhs and rhs refer to the same thread.
 *
 * @param lhs			Left parameter thread
 * @param rhs			Right parameter thread
 * @return				Non-zero value if lhs and rhs refer to the same value, ​0​ otherwise.
 */
int thrd_equal(thrd_t lhs, thrd_t rhs);



/**
 * Terminate calling thread
 *
 * @param rest			the result value to return
 */
void thrd_exit(int res);



/**
 * Blocks the current thread until the thread identified by thr finishes execution.
 * If res is not a null pointer, the result code of the thread is put to the location pointed to by res.
 * The termination of the thread synchronizes-with the completion of this function.
 * The behavior is undefined if the thread was previously detached or joined by another thread.
 *
 * @param thr			identifier of the thread to join
 * @param res			location to put the result code to
 * @return				thrd_success if successful, thrd_error otherwise. 
 */
int thrd_join(thrd_t thr, __OUT__ int* res);



/**
 * Provides a hint to the implementation to reschedule the execution of threads, allowing other threads to run.
 */
void thrd_yield(void);



/**
 * Destroys the mutex pointed to by mutex.
 * If there are threads waiting on mutex, the behavior is undefined.
 *
 * @param mutex			pointer to the mutex to destroy
 */
void mtx_destroy(__OUT__ mtx_t* mutex);



/**
 * Creates a new mutex object with type. The object pointed to by mutex is set to an identifier of the newly created mutex.
 *
 * @param mutex			pointer to the mutex to initialize
 * @param type			the type of the mutex
 * @return				thrd_success if successful, thrd_error otherwise. 
 */
int mtx_init(__OUT__ mtx_t* mutex, int type);



/**
 * Blocks the current thread until the mutex pointed to by mutex is locked.
 * The behavior is undefined if the current thread has already locked the mutex and the mutex is not recursive.
 * Prior calls to mtx_unlock on the same mutex synchronize-with this operation, and all lock/unlock operations on any given mutex form a single total order (similar to the modification order of an atomic)
 *
 * @param mutex			pointer to the mutex to lock
 * @return				thrd_success if successful, thrd_error otherwise
 */
int mtx_lock(__OUT__ mtx_t* mutex);



/**
 * Tries to lock the mutex pointed to by mutex without blocking. Returns immediately if the mutex is already locked.
 * Prior calls to mtx_unlock on the same mutex synchronize-with this operation (if this operation succeeds), and all lock/unlock operations on any given mutex form a single total order (similar to the modification order of an atomic) 
 *
 * @param mutex			pointer to the mutex to lock
 * @return				thrd_success if successful, thrd_busy if the mutex has already been locked, thrd_error if an error occurs. 
 */
int mtx_trylock(__OUT__ mtx_t* mutex);



/**
 * Unlocks the mutex pointed to by mutex.
 * The behavior is undefined if the mutex is not locked by the calling thread.
 * This function synchronizes-with subsequent mtx_lock, mtx_trylock, or mtx_timedlock on the same mutex. All lock/unlock operations on any given mutex form a single total order (similar to the modification order of an atomic).
 *
 * @param mutex			pointer to the mutex to unlock
 * @return				thrd_success if successful, thrd_error otherwise. 
 */
int mtx_unlock(__OUT__ mtx_t* mutex);

#endif /* C11_THREADS_HEADER */
