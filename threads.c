/**
	Library            : Cross Platform C11 Native Threads
	Author             : Gokan EKINCI
	Licence            : CC-BY-NC-ND
	Standard interface : http://en.cppreference.com/w/c/thread
	
	Windows Notes:
		Types : https://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx
	GNU:
		https://www.gnu.org/prep/standards/standards.html
*/
#ifndef C11_THREADS_IMPLEMENTATION
#define C11_THREADS_IMPLEMENTATION
#include "threads.h"

#ifdef __unix__
	#define thrd_errno errno
	typedef void*(*POSIX_START_ROUTINE)(void*);
#endif /* __unix__ */

#ifdef _WIN32
	#define thrd_errno GetLastError()
#endif /* _WIN32 */

#include <errno.h>  /* For errno */



/**
 * Posix: 
 * 		http://man7.org/linux/man-pages/man3/pthread_create.3.html
 * Windows: 
 * 		https://msdn.microsoft.com/fr-fr/library/windows/desktop/ms682516(v=vs.85).aspx
 * 		https://msdn.microsoft.com/fr-fr/library/windows/desktop/ms682453(v=vs.85).aspx
*/
int thrd_create(__OUT__ thrd_t* thr, thrd_start_t func, void* arg) {
	#ifdef __unix__
		int value = pthread_create(thr, NULL, (POSIX_START_ROUTINE) func, arg);
		
		/* ERROR: Setting standard errno with posix value returned from function */
		if (value != 0) {
			errno = value;
			return thrd_error;
		}
	#endif /* __unix__ */

	#ifdef _WIN32
		*thr = CreateThread(
			NULL,							// default security attributes
			0,								// use default stack size
			(LPTHREAD_START_ROUTINE) func,	// thread function name
			arg,							// argument to thread function
			0,								// use default creation flags
			NULL							// returns the thread identifier
		);
		
		/* ERROR: Setting standard errno with windows error value */
		if(*thr == NULL) {
			errno = thrd_errno;
			return thrd_error;
		}
	#endif /* _WIN32 */
	
	/* SUCCESS */
	return thrd_success;
}



/**
 * Posix:	http://man7.org/linux/man-pages/man3/pthread_self.3.html
 * Windows: https://msdn.microsoft.com/en-US/library/windows/desktop/ms683182(v=vs.85).aspx
 */
thrd_t thrd_current(void) {
	#ifdef __unix__
		return pthread_self();
	#endif /* __unix__ */
	
	#ifdef _WIN32
		return GetCurrentThread();
	#endif /* _WIN32 */
}



/**
 * Posix:
 * 		http://man7.org/linux/man-pages/man3/pthread_detach.3.html
 * Windows: 
 * 		https://msdn.microsoft.com/en-us/library/windows/desktop/ms724211.aspx
 * 		http://stackoverflow.com/questions/12744324/how-to-detach-a-thread-on-windows-c
 */
int thrd_detach(thrd_t thr) {
	#ifdef __unix__
		int value = pthread_detach(thr);
		
		/* ERROR: Setting standard errno with posix value returned from function */
		if (value != 0) {
			errno = value;
			return thrd_error;
		}
	#endif /* __unix__ */
	
	#ifdef _WIN32		
		/* Documentation: Closing a thread handle does not terminate the associated thread or remove the thread object. */
		if (CloseHandle(thr) == 0) {
			/* ERROR */
			errno = thrd_errno;
			return thrd_error;
		}
	#endif /* _WIN32 */
	
	/* SUCCESS */
	return thrd_success;
}



/**
 * Windows: https://msdn.microsoft.com/en-US/library/windows/desktop/ms683233(v=vs.85).aspx
 */
int thrd_equal(thrd_t lhs, thrd_t rhs) {
	#ifdef __unix__
		return (lhs == rhs);
	#endif /* __unix__ */
		
	#ifdef _WIN32
		DWORD lhsValue = GetThreadId(lhs);
		if (lhsValue == 0) {
			/* ERROR */
			errno = thrd_errno;
			return thrd_error;
		}
		
		DWORD rhsValue = GetThreadId(rhs);
		if (rhsValue == 0) {
			/* ERROR */
			errno = thrd_errno;
			return thrd_error;
		}
		
		return (lhsValue == rhsValue);
	#endif /* _WIN32 */
}



/**
 * Posix:	http://man7.org/linux/man-pages/man3/pthread_exit.3.html
 * Windows: https://msdn.microsoft.com/fr-fr/library/windows/desktop/ms682659(v=vs.85).aspx
 */
void thrd_exit(int res) {
	#ifdef __unix__
		pthread_exit((void*) &res);
	#endif /* __unix__ */
	
	#ifdef _WIN32
		ExitThread((DWORD) res);
	#endif /* _WIN32 */
}



/**
 * Posix:	http://man7.org/linux/man-pages/man3/pthread_join.3.html
 * Windows: https://msdn.microsoft.com/fr-fr/library/windows/desktop/ms687032(v=vs.85).aspx
 */
int thrd_join(thrd_t thr, __OUT__ int* res) {
	#ifdef __unix__
		int value = pthread_join(thr, (void**) &res);
		
		/* ERROR: Setting standard errno with posix value returned from function */
		if (value != 0) {
			errno = value;
			return thrd_error;
		}
	#endif /* __unix__ */
	
	#ifdef _WIN32		
		DWORD status = WaitForSingleObject(
			thr,
			INFINITE
		);
		
		/* Setting res value */
		if (res != NULL) {
			*res = (int) status;
		}
		
		/* ERROR: Setting standard errno with windows error value */
		if (status != WAIT_OBJECT_0) {
			if (status == WAIT_FAILED) {
				errno = thrd_errno;
			} else {
				errno = status;
			}

			return thrd_error;
		}
	#endif /* _WIN32 */
	
	/* SUCCESS */
	return thrd_success;
}



/**
 * Posix:	https://linux.die.net/man/3/pthread_yield
 * Windows: https://msdn.microsoft.com/en-US/library/windows/desktop/ms686352(v=vs.85).aspx
 */
void thrd_yield(void) {
	#ifdef __unix__
		int value = pthread_yield();

		/* ERROR: Setting standard errno with posix value returned from function */
		if (value != 0) {
			errno = value;
			/* This standard function needs a return value... */
		}
	#endif /* __unix__ */
	
	#ifdef _WIN32
		SwitchToThread();
	#endif /* _WIN32 */
}



/**
 * Posix:	https://linux.die.net/man/3/pthread_mutex_destroy
 * Windows:	https://msdn.microsoft.com/en-US/library/windows/desktop/ms724211(v=vs.85).aspx
 */
void mtx_destroy(__OUT__ mtx_t* mutex) {
	#ifdef __unix__
		int value = pthread_mutex_destroy(mutex);
		
		/* ERROR: Setting standard errno with posix value returned from function */
		if (value != 0) {
			errno = value;
			/* This standard function needs a return value... */
		}
	#endif /* __unix__ */
	
	#ifdef _WIN32
		if (CloseHandle(*mutex) == 0) {
			/* ERROR */
			errno = thrd_errno;
			/* This standard function needs a return value... */
		}
	#endif /* _WIN32 */
}



/**
 * Posix:	https://linux.die.net/man/3/pthread_mutex_init
 * Windows: https://msdn.microsoft.com/en-us/library/windows/desktop/ms682411(v=vs.85).aspx
 */
int mtx_init(__OUT__ mtx_t* mutex, int type) {
	#ifdef __unix__
		int value = 0;
		if (type == (mtx_plain | mtx_recursive)) {
			pthread_mutexattr_t properties;
			pthread_mutexattr_init(&properties);
			pthread_mutexattr_settype(&properties, PTHREAD_MUTEX_RECURSIVE);
			value = pthread_mutex_init(mutex, &properties);
		} else if (type == mtx_plain) {
			value = pthread_mutex_init(mutex, NULL);
		} else {
			/* ERROR: Unsupported type parameter */
			errno = EINVAL;
			return thrd_error;
		}
		
		/* ERROR: Setting standard errno with posix value returned from function */
		if (value != 0) {
			errno = value;
			return thrd_error;
		}
	#endif /* __unix__ */
	
	#ifdef _WIN32
		/* ERROR: Unsupported type parameter */
		if (type != mtx_plain || type != (mtx_plain | mtx_recursive)) {
			return thrd_error;
		}
		
		BOOL owner = ((type & mtx_recursive) == mtx_recursive);
		*mutex = CreateMutex(
			NULL,				// default security attributes
			owner,				// owned if true
			NULL				// unnamed mutex
		);
		
		/* ERROR: Setting standard errno with windows error value */
		if(*mutex == NULL) {
			errno = thrd_errno;
			return thrd_error;
		}
	#endif /* _WIN32 */
	
	/* SUCCESS */
	return thrd_success;
}


/**
 * Posix:	https://linux.die.net/man/3/pthread_mutex_lock
 * Windows:	https://msdn.microsoft.com/en-US/library/windows/desktop/ms687032(v=vs.85).aspx
 */
int mtx_lock(__OUT__ mtx_t* mutex) {
	#ifdef __unix__
		int value = pthread_mutex_lock(mutex);
		
		/* ERROR: Setting standard errno with posix value returned from function */
		if (value != 0) {
			errno = value;
			return thrd_error;
		}
	#endif /* __unix__ */
	
	#ifdef _WIN32
		DWORD status = WaitForSingleObject(*mutex, INFINITE);
		
		/* ERROR: Setting standard errno with windows error value */
		if (status != WAIT_OBJECT_0) {
			if (status == WAIT_FAILED) {
				errno = thrd_errno;
			} else {
				errno = status;
			}

			return thrd_error;
		}
	#endif /* _WIN32 */
	
	/* SUCCESS */
	return thrd_success;
}



/**
 * Posix:	https://linux.die.net/man/3/pthread_mutex_trylock
 * Windows:	https://msdn.microsoft.com/en-US/library/windows/desktop/ms687032(v=vs.85).aspx
 */
int mtx_trylock(__OUT__ mtx_t* mutex) {
	#ifdef __unix__
		int value = pthread_mutex_trylock(mutex);
		
		/* ERROR: Setting standard errno with posix value returned from function */
		if (value != 0) {
			errno = value;
			return (value == EBUSY) ? thrd_busy : thrd_error;
		}
	#endif /* __unix__ */
	
	#ifdef _WIN32
		DWORD status = WaitForSingleObject(*mutex, INFINITE);
		
		/* ERROR: Setting standard errno with windows error value */
		if (status != WAIT_OBJECT_0) {
			if (status == WAIT_TIMEOUT) {
				errno = status;
				return thrd_busy;
			} else if (status == WAIT_FAILED) {
				errno = thrd_errno;
				return thrd_error;
			} else {
				errno = status;
				return thrd_error;
			}			
		}
	#endif /* _WIN32 */
	
	/* SUCCESS */
	return thrd_success;
}



/**
 * Posix:	https://linux.die.net/man/3/pthread_mutex_unlock
 * Windows: https://msdn.microsoft.com/en-US/library/windows/desktop/ms685066(v=vs.85).aspx
 */
int mtx_unlock(__OUT__ mtx_t* mutex) {
	#ifdef __unix__
		int value = pthread_mutex_unlock(mutex);
		
		/* ERROR: Setting standard errno with posix value returned from function */
		if (value != 0) {
			errno = value;
			return thrd_error;
		}
	#endif /* __unix__ */

	#ifdef _WIN32
		if (ReleaseMutex(*mutex) == 0) {
			/* ERROR */
			errno = thrd_errno;
			return thrd_error;
		}
	#endif /* _WIN32 */

	/* SUCCESS */
	return thrd_success;
}

#endif /* C11_THREADS_IMPLEMENTATION */
