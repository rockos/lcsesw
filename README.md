## NODE.JSのアドオンモジュール作成

### ビルド環境の構築
#### MacOSxLion のケース
node.jsのバージョンはv.0.8以上で、python v2以上であること。

	$ node -v
	v0.8.12

	$ python -V
	Python 2.7.1

node-gypをnpmでグローバルインストールします。

	$ sudo npm install -g node-gyp

#### サンプルのビルドが可能か?
NODE.JSのソースに添付されているサンプルソースがビルド可能か確認します。

	$ cd /Users/sin0414/nvm/src/node-v0.8.9/test/addons/hello-world


test.jsが正常に動けばビルド環境は正常に構築できている。

	hello-world $ node test.js
	binding.hello() = world

### オリジナルのテストモジュール作成
まず、モジュールファイル hello.cc を作成します。
ファイル名はなんでもよく、拡張子をccとします。


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


binding.gypファイルの作成

モジュールをmakeするときに必要なファイルです。ファイル名はbinding.gypでなければなりません。

	{
	    "targets": [
	    {
	        "target_name": "hello",
	            "sources": ["hello.cc"]
	    }
	    ]
	}


最後に上のモジュールを使用するjavascriptファイルtest.jsを作成します。

	var lcsEsw = require('./build/Release/hello');
	console.log('hello() = ' + lcsEsw.sayHello());
	console.log('what() = ' + lcsEsw.sayWhat());

#### モジュールのビルド
作成したファイルをビルドします。

	 $ node-gyp configure build

ビルドが成功すると、<code>./build/Release/</code>に*hello.node*ができています。

#### モジュールの動作確認
	$ node test
	
	hello() = Hello world!!
	what() = what your name!!
	 



参考
https://c9.io/leeolayvar/fenearstreamer/workspace/node_modules/forever/node_modules/daemon/src/daemon.cc



##### win32 API使用例
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



実行してみる。
	
	$ node test.js
	send message
	
	$ log trace -l 10
	
	2012/10/25 08:00:10.000 [lcsEsw.send]
	

	

	
