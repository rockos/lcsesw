## NODE.JS�̃A�h�I�����W���[���쐬

### �r���h���̍\�z
#### MacOSxLion �̃P�[�X
node.js�̃o�[�W������v.0.8�ȏ�ŁApython v2�ȏ�ł��邱�ƁB

	$ node -v
	v0.8.12

	$ python -V
	Python 2.7.1

node-gyp��npm�ŃO���[�o���C���X�g�[�����܂��B

	$ sudo npm install -g node-gyp

#### �T���v���̃r���h���\��?
NODE.JS�̃\�[�X�ɓY�t����Ă���T���v���\�[�X���r���h�\���m�F���܂��B

	$ cd /Users/sin0414/nvm/src/node-v0.8.9/test/addons/hello-world


test.js������ɓ����΃r���h���͐���ɍ\�z�ł��Ă���B

	hello-world $ node test.js
	binding.hello() = world

### �I���W�i���̃e�X�g���W���[���쐬
�܂��A���W���[���t�@�C�� hello.cc ���쐬���܂��B
�t�@�C�����͂Ȃ�ł��悭�A�g���q��cc�Ƃ��܂��B


	#include <node.h>
	#include <v8.h>

	using namespace v8;

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


binding.gyp�t�@�C���̍쐬

���W���[����make����Ƃ��ɕK�v�ȃt�@�C���ł��B�t�@�C������binding.gyp�łȂ���΂Ȃ�܂���B

	{
	    "targets": [
	    {
	        "target_name": "hello",
	            "sources": ["hello.cc"]
	    }
	    ]
	}


�Ō�ɏ�̃��W���[�����g�p����javascript�t�@�C��test.js���쐬���܂��B

	var lcsEsw = require('./build/Release/hello');
	console.log('hello() = ' + lcsEsw.sayHello());
	console.log('what() = ' + lcsEsw.sayWhat());

#### ���W���[���̃r���h
�쐬�����t�@�C�����r���h���܂��B

	 $ node-gyp configure build

�r���h����������ƁA<code>./build/Release/</code>��*hello.node*���ł��Ă��܂��B

#### ���W���[���̓���m�F
	$ node test
	
	hello() = Hello world!!
	what() = what your name!!
	 



�Q�l
https://c9.io/leeolayvar/fenearstreamer/workspace/node_modules/forever/node_modules/daemon/src/daemon.cc



##### win32 API�g�p��
lcsesw.cc


	#include <node.h>  
	#include <v8.h>
	#include <stdlib.h>
	#include <string.h>

	using namespace v8;
	#define FALSE	0
	#define TRUE	1
	#define ACCESS_ALL	1
	/*
	 * Send message into mailbox and/or turn on event flag
 	 */
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

binding.gyp


	{
    	"targets": [
    	{
        	"target_name": "lcsEsw",
            	"sources": ["lcsesw.cc"]
    	}
    	]
	}

test.js

	var lcsEsw = require('./build/Release/lcsesw');
	try {
		lcsEsw.send('qwer tyu');
	} catch(e) {
		console.log('error' +e.message);
	}
	console.log('send message');



���s���Ă݂�B
	
	$ node test.js
	send message
	
	$ log trace -l 10
	
	2012/10/25 08:00:10.000 [lcsEsw.send]
	

	

	
