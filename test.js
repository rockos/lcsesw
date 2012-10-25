var lcsEsw = require('./build/Release/lcsesw');
try {
	lcsEsw.send('qwer tyu');
} catch(e) {
	console.log('error' +e.message);
}
console.log('send message');

