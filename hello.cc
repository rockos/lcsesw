#include <node.h>
#include <v8.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace v8;
extern "C" {
#define FALSE	0
#define TRUE	1
#define ACCESS_ALL	1

typedef struct  t_table {
	char    name[ 16 ];		/* process name */
	char    mbxname[ 64 ];		/* mailbox name */
	HANDLE	mtx;	// mutex object handle
	HANDLE	efn;	// event object handle
	HANDLE	mbx;		// mailbox object handle (read only).
	OVERLAPPED	ovlp;		/* for mailbox async. read */
	int mbxSiz;
}       PRCTBL;

typedef struct  t_context       {
	int alive;		/* Am I alive? */
	int	id;
	void    *gcAdr[ 2 ];
	long    maxTbl;
	PRCTBL   *tbl;		/* for IPC  define in fw.h */
	HANDLE	ownMbx;		// mailbox object handle.
	OVERLAPPED	ownOv;		/* for mailbox async. read */
	HANDLE	ownMtx;		// mutex object handle
	HANDLE	ownEfn;		// event object handle
	int		ownSiz;		/* own mbx size */
}       CONTEXT2;

static  CONTEXT2 ctx;

/***********************************************************************
	Create name table
***********************************************************************/

	
static BOOL setPriv(
	HANDLE hToken,			// token handle
	LPCTSTR Privilege,		// Privilege to enable/disable
	BOOL bEnablePrivilege )	// TRUE to enable.  FALSE to disable
	{
		TOKEN_PRIVILEGES tp;
		LUID luid;
		TOKEN_PRIVILEGES tpPrevious;
		DWORD cbPrevious=sizeof(TOKEN_PRIVILEGES);

		if(!LookupPrivilegeValue( NULL, Privilege, &luid ))
			return FALSE;

		//
		// first pass.  get current privilege setting
		//
		tp.PrivilegeCount           = 1;
		tp.Privileges[0].Luid       = luid;
		tp.Privileges[0].Attributes = 0;

		AdjustTokenPrivileges(
				hToken,
				FALSE,
				&tp,
				sizeof(TOKEN_PRIVILEGES),
				&tpPrevious,
				&cbPrevious
				);

		if (GetLastError() != ERROR_SUCCESS) return FALSE;

		//
		// second pass.  set privilege based on previous setting
		//
		tpPrevious.PrivilegeCount       = 1;
		tpPrevious.Privileges[0].Luid   = luid;

		if(bEnablePrivilege) {
			tpPrevious.Privileges[0].Attributes |= 
				(SE_PRIVILEGE_ENABLED);
		}
		else {
			tpPrevious.Privileges[0].Attributes ^= 
				(SE_PRIVILEGE_ENABLED &
					tpPrevious.Privileges[0].Attributes);
		}

		AdjustTokenPrivileges(
				hToken,
				FALSE,
				&tpPrevious,
				cbPrevious,
				NULL,
				NULL
				);

		if (GetLastError() != ERROR_SUCCESS) return FALSE;

		return TRUE;
	}

	
static int openProc( int id, PHANDLE phProc )
{
	BOOL		bRc;
	HANDLE		hToken;

	bRc = OpenProcessToken( GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
			&hToken );
	if( !bRc ){
		// LOGERR( "OpenProcessToken", GetLastError());
	}

	// enable SeDebugPrivilege
	bRc = setPriv(hToken, SE_DEBUG_NAME, TRUE);
	if ( !bRc ) {
		// LOGERR( "setPriv", GetLastError());
		// close token handle
		CloseHandle(hToken);
		// indicate failure
		return FALSE;
	}

	//-------------------------------------
	//	プロセスのオープン
	//-------------------------------------
	*phProc = OpenProcess
		( PROCESS_DUP_HANDLE | PROCESS_ALL_ACCESS, FALSE, id);
	// disable SeDebugPrivilege
	setPriv(hToken, SE_DEBUG_NAME, FALSE);
	if( *phProc == NULL ) {
		// LOGERR( "setPriv", GetLastError() );
		return (FALSE);
	}

	// close handles
	CloseHandle(hToken);

	return TRUE;
}
}
Handle<Value> sayWhat(const Arguments& args) {
	HandleScope scope;
	return scope.Close(String::New("what your name!!"));
}
Handle<Value> sayHello(const Arguments& args) {
	HandleScope scope;
	return scope.Close(String::New("Hello 	world!!"));
}

void init(Handle<Object> target) {
	target->Set(String::NewSymbol("sayWhat"),
			FunctionTemplate::New(sayWhat)->GetFunction());

	target->Set(String::NewSymbol("sayHello"),
			FunctionTemplate::New(sayHello)->GetFunction());
}
NODE_MODULE(hello, init);
