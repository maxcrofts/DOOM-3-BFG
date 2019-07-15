/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").  

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#ifndef __THREAD_H__
#define __THREAD_H__

/*
================================================
idSysMutex provides a C++ wrapper to the low level system mutex functions.  A mutex is an
object that can only be locked by one thread at a time.  It's used to prevent two threads
from accessing the same piece of data simultaneously.
================================================
*/
class idSysMutex {
public:
					idSysMutex() { Sys_MutexCreate( handle ); }
					~idSysMutex() { Sys_MutexDestroy( handle ); }

	bool			Lock( bool blocking = true ) { return Sys_MutexLock( handle, blocking ); }
	void			Unlock() { Sys_MutexUnlock( handle ); }

private:
	mutexHandle_t	handle;

					idSysMutex( const idSysMutex & s ) {}
	void			operator=( const idSysMutex & s ) {}
};

/*
================================================
idScopedCriticalSection is a helper class that automagically locks a mutex when it's created
and unlocks it when it goes out of scope.
================================================
*/
class idScopedCriticalSection {
public:
	idScopedCriticalSection( idSysMutex & m ) : mutex(&m) { mutex->Lock(); }
	~idScopedCriticalSection() { mutex->Unlock(); }

private:
	idSysMutex *	mutex;	// NOTE: making this a reference causes a TypeInfo crash
};

/*
================================================
idSysSignal is a C++ wrapper for the low level system signal functions.  A signal is an object
that a thread can wait on for it to be raised.  It's used to indicate data is available or that
a thread has reached a specific point.
================================================
*/
class idSysSignal {
public:
	static const int	WAIT_INFINITE = -1;

			idSysSignal( bool manualReset = false );
			~idSysSignal();

	void	Raise();
	void	Clear();

	// Wait returns true if the object is in a signalled state and
	// returns false if the wait timed out. Wait also clears the signalled
	// state when the signalled state is reached within the time out period.
	bool	Wait( int timeout = WAIT_INFINITE );

private:
	bool				manualReset;
	bool				signaled;
	mutexHandle_t		mutex;
	condHandle_t		condition;

						idSysSignal( const idSysSignal & s ) {}
	void				operator=( const idSysSignal & s ) {}
};

/*
================================================
idSysInterlockedInteger is a C++ wrapper for the low level system interlocked integer
routines to atomically increment or decrement an integer.
================================================
*/
class idSysInterlockedInteger {
public:
						idSysInterlockedInteger() : value( 0 ) {}

	// atomically increments the integer and returns the new value
	int					Increment() { return Sys_InterlockedIncrement( value ); }

	// atomically decrements the integer and returns the new value
	int					Decrement() { return Sys_InterlockedDecrement( value ); }

	// atomically adds a value to the integer and returns the new value
	int					Add( int v ) { return Sys_InterlockedAdd( value, (interlockedInt_t) v ); }

	// atomically subtracts a value from the integer and returns the new value
	int					Sub( int v ) { return Sys_InterlockedSub( value, (interlockedInt_t) v ); }

	// returns the current value of the integer
	int					GetValue() const { return value; }

	// sets a new value, Note: this operation is not atomic
	void				SetValue( int v ) { value = (interlockedInt_t)v; }

private:
	interlockedInt_t	value;
};

/*
================================================
idSysThread is an abstract base class, to be extended by classes implementing the
idSysThread::Run() method. 

	class idMyThread : public idSysThread {
	public:
		virtual int Run() {
			// run thread code here
			return 0;
		}
		// specify thread data here
	};

	idMyThread thread;
	thread.Start( "myThread" );

A worker thread is a thread that waits in place (without consuming CPU)
until work is available. A worker thread is implemented as normal, except that, instead of
calling the Start() method, the StartWorker() method is called to start the thread.
Note that the Sys_CreateThread function does not support the concept of worker threads.

	class idMyWorkerThread : public idSysThread {
	public:
		virtual int Run() {
			// run thread code here
			return 0;
		}
		// specify thread data here
	};

	idMyWorkerThread thread;
	thread.StartThread( "myWorkerThread" );
 
	// main thread loop
	for ( ; ; ) {
		// setup work for the thread here (by modifying class data on the thread)
		thread.SignalWork();           // kick in the worker thread
		// run other code in the main thread here (in parallel with the worker thread)
		thread.WaitForThread();        // wait for the worker thread to finish
		// use results from worker thread here
	}

In the above example, the thread does not continuously run in parallel with the main Thread,
but only for a certain period of time in a very controlled manner. Work is set up for the
Thread and then the thread is signalled to process that work while the main thread continues.
After doing other work, the main thread can wait for the worker thread to finish, if it has not
finished already. When the worker thread is done, the main thread can safely use the results
from the worker thread.

Note that worker threads are useful on all platforms but they do not map to the SPUs on the PS3.
================================================
*/
class idSysThread {
public:
					idSysThread();
	virtual			~idSysThread();

	const char *	GetName() const { return name.c_str(); }
	uintptr_t		GetThreadHandle() const { return threadHandle; }
	bool			IsRunning() const { return isRunning; }
	bool			IsTerminating() const { return isTerminating; }

	//------------------------
	// Thread Start/Stop/Wait
	//------------------------

	bool			StartThread( const char * name, core_t core, 
								 xthreadPriority priority = THREAD_NORMAL,
								 int stackSize = DEFAULT_THREAD_STACK_SIZE );

	bool			StartWorkerThread( const char * name, core_t core, 
									   xthreadPriority priority = THREAD_NORMAL,
									   int stackSize = DEFAULT_THREAD_STACK_SIZE );

	void			StopThread( bool wait = true );

	// This can be called from multiple other threads. However, in the case
	// of a worker thread, the work being "done" has little meaning if other
	// threads are continuously signalling more work.
	void			WaitForThread();

	//------------------------
	// Worker Thread
	//------------------------

	// Signals the thread to notify work is available.
	// This can be called from multiple other threads.
	void			SignalWork();

	// Returns true if the work is done without waiting.
	// This can be called from multiple other threads. However, the work
	// being "done" has little meaning if other threads are continuously
	// signalling more work.
	bool			IsWorkDone();

protected:
	// The routine that performs the work.
	virtual int		Run();

private:
	idStr			name;
	xthreadPriority	priority;
	uintptr_t		threadHandle;
	bool			isWorker;
	bool			isRunning;
	volatile bool	isTerminating;
	volatile bool	moreWorkToDo;
	idSysSignal		signalWorkerDone;
	idSysSignal		signalMoreWorkToDo;
	idSysMutex		signalMutex;

	static int		ThreadProc( idSysThread * thread );

					idSysThread( const idSysThread & s ) {}
	void			operator=( const idSysThread & s ) {}
};

#endif // !__THREAD_H__
