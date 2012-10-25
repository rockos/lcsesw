#include <node.h>
#include <v8.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace v8;
#define FALSE	0
#define TRUE	1
#define ACCESS_ALL	1
/***********************************************************************
  Send message into mailbox and/or turn on event flag
 ***********************************************************************/
Handle<Value> fnFoward(const Arguments& args) {
	HandleScope scope;
	HANDLE hmbx;
	DWORD len, cnt = 0;
	char message[256];

	if (args.Length() < 2) {
		return scope.Close(Boolean::New(false));
	}
	memset(message, 0, sizeof(message));
	len = args[1]->Int32Value();
	if (len < 1) {
		return ThrowException(Exception::Error(
					String::New("Error invalid length")));
	}
	strcpy(message, *String::Utf8Value(args[0]));

	while (TRUE) {
		hmbx = CreateFile("\\\\.\\mailslot\\SYSMSG",
				GENERIC_WRITE,
				FILE_SHARE_READ,
				(LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				(HANDLE)NULL );

		if ( hmbx == INVALID_HANDLE_VALUE ) {
			cnt++;
			if ( cnt > 5 ) {
				return scope.Close(Boolean::New(false));
			}
			Sleep(30);
			continue;
		}
		break;
	}
	if (! WriteFile(hmbx, message,
				len,
				&len, (LPOVERLAPPED)NULL )) {
		return scope.Close(Boolean::New(false));
	}
	return scope.Close(Boolean::New(true));
}
extern "C"
void init(Handle<Object> target) {

	NODE_SET_METHOD(target, "send", fnFoward);
}
NODE_MODULE(lcsesw, init);

