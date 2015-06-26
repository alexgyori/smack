#include "pthread.h"

__SMACK_INIT(corral_primitives) {
  //Declare these, so bpl parsing doesn't complain
  __SMACK_top_decl("procedure corral_getThreadID() returns (x:int);");
  __SMACK_top_decl("procedure corral_getChildThreadID() returns (x:int);");
  __SMACK_top_decl("procedure corral_atomic_begin();");
  __SMACK_top_decl("procedure corral_atomic_end();");
}

__SMACK_INIT(thread) {
  //Array and possible statuses for tracking pthreads
  __SMACK_top_decl("//dim0=tid, dim1= idx 0 gets status, 1 gets return value");
  __SMACK_top_decl("var $pthreadStatus: [int][int]int;");
  __SMACK_top_decl("const unique $pthread_uninitialized: int;");
  __SMACK_top_decl("const unique $pthread_initialized: int;");
  __SMACK_top_decl("const unique $pthread_waiting: int;");
  __SMACK_top_decl("const unique $pthread_running: int;");
  __SMACK_top_decl("const unique $pthread_stopped: int;");
  // Initialize this array so all threads begin as uninitialized
  __SMACK_code("assume (forall i:int :: $pthreadStatus[i][0] == $pthread_uninitialized);"); 
}

pthread_t pthread_self(void) {
  int tmp_tid = __SMACK_nondet();
  __SMACK_code("call @ := corral_getThreadID();", tmp_tid);

  // Print actual tid to SMACK traces
  int actual_tid = tmp_tid;

  return actual_tid;
}

int pthread_join(pthread_t __th, void **__thread_return) {
  // Print the tid of the thread being joined to SMACK traces
  void* joining_tid = (void*)__th;

  // Wait for the thread to terminate
  __SMACK_code("assume $pthreadStatus[@][0] == $pthread_stopped;", __th);

  // Get the thread's return value
  void* tmp_thread_return_pointer = (void*)__SMACK_nondet();
  __SMACK_code("@ := $pthreadStatus[@][1];", tmp_thread_return_pointer, __th);
  *__thread_return = tmp_thread_return_pointer;

  // Print return pointer value to SMACK traces
  void* actual_thread_return_pointer = *__thread_return;

  return 0;
}

void pthread_exit(void *retval) {
  pthread_t tid = pthread_self();

  // Ensure exit hasn't already been called
  __SMACK_code("assert $pthreadStatus[@][0] == $pthread_running;", tid);
  __SMACK_code("$pthreadStatus[@][1] := @;", tid, retval);

  // Set return pointer value for display in SMACK traces
  void* pthread_return_pointer = retval;
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
  attr->type = PTHREAD_MUTEX_NORMAL;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
  attr->type = type;
  return 0;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
  // Can't check for already initialized error, 
  //  since uninitialized values are nondet and could be INITIALIZED
  mutex->lock = UNLOCKED;
  mutex->init = INITIALIZED;
  if(attr == 0) {
    pthread_mutexattr_init(&mutex->attr);
  } else {
    mutex->attr.type = attr->type;
    // Any additional info modeled from attr should be copied,
    //  as changes to attr should not apply to mutexes alread initialized
    //  (as opposed to directly setting mutex->attr = attr)
  }
  return 0;
}

int pthread_mutex_lock(pthread_mutex_t *__mutex) {
  int tid = (int)pthread_self();
  // Ensure mutex is initialized & hasn't already been locked by caller
  if(__mutex->attr.type==PTHREAD_MUTEX_NORMAL) {
    assert(__mutex->init == INITIALIZED);
    assert(__mutex->lock != tid);
  } else if(__mutex->attr.type==PTHREAD_MUTEX_ERRORCHECK) {
    if(__mutex->init != INITIALIZED)
      return 22;    // This is EINVAL
    if(__mutex->lock == tid)
      return 35;    // This is EDEADLK
  } else {
    // Other types not currently implemented
    assert(0);
  }
  __SMACK_code("call corral_atomic_begin();");
  // Wait for lock to become free
  assume(__mutex->lock == UNLOCKED);
  __mutex->lock = tid;
  __SMACK_code("call corral_atomic_end();");
  return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *__mutex) {
  int tid = (int)pthread_self();
  // Ensure mutex is initialized & caller is current owner
  if(__mutex->attr.type==PTHREAD_MUTEX_NORMAL) {
    assert(__mutex->init == INITIALIZED);
    assert(__mutex->lock == tid);
  } else if(__mutex->attr.type==PTHREAD_MUTEX_ERRORCHECK) {
    if(__mutex->init != INITIALIZED)
      return 22;    // This is EINVAL
    if(__mutex->lock != tid) 
      return 1;     // This is EPERM
  } else {
    // Other types not currently implemented
    assert(0);
  }
  __SMACK_code("call corral_atomic_begin();");
  __mutex->lock = UNLOCKED;
  __SMACK_code("call corral_atomic_end();");
  return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *__mutex) {
  // Make sure the lock is initialized, and unlocked
  assert(__mutex->init == INITIALIZED);
  assert(__mutex->lock == UNLOCKED);
  __SMACK_code("call corral_atomic_begin();");
  __mutex->init = UNINITIALIZED;
  __SMACK_code("call corral_atomic_end();");
  return 0;
}

int pthread_cond_init(pthread_cond_t *__cond, pthread_condattr_t *__condattr) {
  if(__condattr == 0) {
    __cond->cond = 0;
    __cond->init = INITIALIZED;
  } else {
    // Unimplemented
    // NOTE: if implemented, attr should be a copy of __condattr passed in
    //       (spec says changes to condattr doesn't affect initialized conds
    assert(0);
  }
  return 0;  
}

int pthread_cond_wait(pthread_cond_t *__cond, pthread_mutex_t *__mutex) {
  // Ensure conditional var is initialized, and mutex is locked properly
  assert(__cond->init == INITIALIZED);
  assert((int)pthread_self() == __mutex->lock);
  pthread_mutex_unlock(__mutex);
  // Wait to be woken up
  // No need to check for signal on __cond, since OS can do spurious wakeup
  // Call to pthread_cond_wait should always be protected by while loop

  // Adding var checks in improves performance

  //assume(__cond->cond == 1);
  //__cond->cond = 0;
  pthread_mutex_lock(__mutex);
}

int pthread_cond_signal(pthread_cond_t *__cond) {
  // Do nothing, since state of __cond doesn't matter
  //  due to possibility of spurious wakeup from OS

  //__cond->cond = 1;
}

// NOTE: How to handle broadcast?  Technically, as is, all threads have
//       the opportunity to read __cond->cond before one of the other threads
//       switch this value back to 0...  I guess this is sufficient, but
//       will this require a high number of context switches for a true
//       broadcast to happen?
//
//       I thought about using cond->cond = 2 to signal broadcast, but then
//       how to know which thread to have switch it back to 0?

int pthread_cond_broadcast(pthread_cond_t *__cond) {
  // Do nothing, since state of __cond doesn't matter
  //  due to possibility of spurious wakeup from OS

  //__cond->cond = 1;
}

int pthread_cond_destroy(pthread_cond_t *__cond) {
  // Make sure the cond is initialized
  assert(__cond->init == INITIALIZED);
  __cond->init = UNINITIALIZED;
  return 0;
}

void __call_wrapper(pthread_t *__restrict __newthread,
                    void *(*__start_routine) (void *),
                    void *__restrict __arg) {

  pthread_t ctid = pthread_self();
  assume(ctid == *__newthread);
  
  // I think Zvonimir proposed to just use ctid to index into $pthreadStatus
  // This would work in most situations, HOWEVER, what if the parameter __arg
  // points to __newthread?  Then, __start_routine() could modify this
  // object before the parent thread sets __newthread to the ctid.
  __SMACK_code("$pthreadStatus[@][0] := $pthread_waiting;", *__newthread);

  __SMACK_code("$pthreadStatus[@][0] := $pthread_running;", *__newthread);
  __start_routine(__arg);
  __SMACK_code("$pthreadStatus[@][0] := $pthread_stopped;", *__newthread);
}

int pthread_create(pthread_t *__restrict __newthread,
                   __const pthread_attr_t *__restrict __attr,
                   void *(*__start_routine) (void *),
                   void *__restrict __arg) {

  pthread_t tmp = __SMACK_nondet();

  // Add unreachable C-level call to __call_wrapper, so llvm sees
  // the call to __call_wrapper and performs DSA on it.
  int x = __SMACK_nondet();
  assume(x == 0);
  if(x) __call_wrapper(__newthread, __start_routine, __arg);

  __SMACK_code("async call @(@, @, @);",
               __call_wrapper, __newthread,
               __start_routine, __arg);
  __SMACK_code("call @ := corral_getChildThreadID();", tmp);
  *__newthread = tmp;

  return 0;
}