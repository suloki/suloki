/*
 *  C eventloop example (c_eventloop.c).
 *
 *  Ecmascript code to initialize the exposed API (setTimeout() etc) when
 *  using the C eventloop.
 *
 *  https://developer.mozilla.org/en-US/docs/Web/JavaScript/Timers
 */

/*
 *  Timer API
 */
 
 Duktape.modSearch = function (id) {
    //print('loading module:', id);
    // Return source code for module or throw an error.
	//
	var res;
    print('loading module:'+id);
    res = loadfile(id);
    if (typeof res === 'string') {
		print('load ok');
        return res;
    }
	print('load err');
    throw new Error('module not found: ' + id);
	//
	/*
	print('loading module:'+id);
	if (id === 'bar') {
        return 'exports.hello = function() { print("Hello from bar!"); };';
    }
    throw new Error('module not found: ' + id);
	*/
};

var g_moduleName;// = 'jsmodulehandler';
var g_root;
var g_sequenceid=0;
var modProtoBuf = require('./jspublicmodules/protobuf.js');
modProtoBuf.load("./proto/suloki.proto", function(err, root) {
	if (err)
        	throw err;
	g_root = root;
    // Obtain a message type
    var AwesomeMessage = root.lookupType("suloki.SulokiMessage");
    // Exemplary payload
    var payload = { protocolVersion: "1.0",businessId:1,messageId:2,messageType:0,sequenceNumber:3};
    // Verify the payload if necessary (i.e. when possibly incomplete or invalid)
    var errMsg = AwesomeMessage.verify(payload);
    if (errMsg)
        throw Error(errMsg);
});
var test_number = 0;
var test_timestart;
var test_print = 0;
EventLoop.callbackOwner=function(msg, userdata, moduleNameFrom) {
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	var resMsg = AwesomeMessage.decode(msg);
	//test function and performance
	if(resMsg['businessId'] == 0 && resMsg['messageId'] == 8 && resMsg['sequenceNumber'] == 0)
	{
		{
		var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
		var payload = {protocolVersion: "2.0.0.0",businessId:4,messageId:1,messageType:0,sequenceNumber:g_sequenceid++};
		var message = AwesomeMessage.create(payload);
		{
			var AwesomeMessageBody = g_root.lookupType("suloki.SulokiCacheBody");
			var payload = {key:"0_"+g_moduleName,val:"jsVal"};//,jscallback:"callback1"};
			var body = AwesomeMessageBody.create(payload);
			//print(body['jscallback']);
			var buffer = AwesomeMessageBody.encode(body).finish();
			message['messageBody'] = buffer;
		}
		var buffer = AwesomeMessage.encode(message).finish();
		Suloki_OperateCache(buffer, g_moduleName);
		}
		return 0;
	}
	if(resMsg['businessId'] == 0 && resMsg['messageId'] == 8 && resMsg['sequenceNumber'] == 30)
	{
		var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
		//g_sequenceid = g_sequenceid+1;
		var payload = {protocolVersion: "2.0.0.0",businessId:0,messageId:0,messageType:0,sequenceNumber:resMsg['sequenceNumber']};
		var message = AwesomeMessage.create(payload);
		var buffer = AwesomeMessage.encode(message).finish();
		var resMsgData = Suloki_Request(buffer, 'iomoduletest', 0, g_moduleName, 3000);
		if(!isNaN(resMsgData))
		{
			print('Suloki_Request error '+resMsgData);
		}
		else
		{
			var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
			var resMsg = AwesomeMessage.decode(resMsgData);
			{
			var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
			var payload = {protocolVersion: "2.0.0.0",businessId:4,messageId:1,messageType:0,sequenceNumber:g_sequenceid++};
			var message = AwesomeMessage.create(payload);
			{
				var AwesomeMessageBody = g_root.lookupType("suloki.SulokiCacheBody");
				var payload = {key:"30_"+g_moduleName,val:"jsVal"};//,jscallback:"callback1"};
				var body = AwesomeMessageBody.create(payload);
				//print(body['jscallback']);
				var buffer = AwesomeMessageBody.encode(body).finish();
				message['messageBody'] = buffer;
			}
			var buffer = AwesomeMessage.encode(message).finish();
			Suloki_OperateCache(buffer, g_moduleName);
			}
		}
		return 0;
	}
	if(resMsg['businessId'] == 0 && resMsg['messageId'] == 0 && resMsg['messageType'] == 2 && resMsg['sequenceNumber'] >= 1000000)
	{
		if(test_number == 0)
			test_timestart = new Date();
		test_number++;
		if((new Date())-test_timestart >= 1000 && test_print == 0)
		{
			test_print = 1;
			print(g_moduleName+' recv notice '+test_number/1+'/s');
		}
		return 0;
	}
	if(resMsg['businessId'] == 0 && resMsg['messageId'] == 0 && resMsg['messageType'] == 0 && resMsg['sequenceNumber'] >= 100)
	{
		var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
		var payload = {protocolVersion: "2.0.0.0",businessId:0,messageId:0,messageType:0,sequenceNumber:g_sequenceid++};
		var message = AwesomeMessage.create(payload);
		var buffer = AwesomeMessage.encode(message).finish();
		var resCode1 = Suloki_RequestDirectToIomodule('iomoduletest', buffer, 3000, g_moduleName);
		if(isNaN(resCode1))
		{
			var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
			var resMsg1 = AwesomeMessage.decode(resCode1);
			if(resMsg1['errorCode'] == 0)
			{
				resMsg['messageType'] = 1;
				resMsg['errorCode'] = 0;
				var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
				var buffer = AwesomeMessage.encode(resMsg).finish();
				var resMsgData = Suloki_Response(buffer, moduleNameFrom, 0, g_moduleName);
			}
			else
				print('Suloki_RequestDirectToIomodule error1');
		}
		return 0;
	}
	if(resMsg['businessId'] == 0 && resMsg['messageId'] == 0 && resMsg['messageType'] == 0)
	{
		resMsg['messageType'] = 1;
		resMsg['errorCode'] = 0;
		var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
		var buffer = AwesomeMessage.encode(resMsg).finish();
		var resMsgData = Suloki_Response(buffer, moduleNameFrom, 0, g_moduleName);
		return 0;
	}
	if(resMsg['businessId'] == 0 && resMsg['messageId'] == 8 && resMsg['sequenceNumber'] == 40)
	{
		var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
		var payload = {protocolVersion: "2.0.0.0",businessId:0,messageId:0,messageType:0,sequenceNumber:g_sequenceid++};
		var message = AwesomeMessage.create(payload);
		var buffer = AwesomeMessage.encode(message).finish();
		var resCode = Suloki_RequestAsync(buffer, 'iomoduletest', 0, g_moduleName, 3000, 0, function(resData,userData,moduleFrom){
			{
			var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
			var payload = {protocolVersion: "2.0.0.0",businessId:4,messageId:1,messageType:0,sequenceNumber:g_sequenceid++};
			var message = AwesomeMessage.create(payload);
			{
				var AwesomeMessageBody = g_root.lookupType("suloki.SulokiCacheBody");
				var payload = {key:"40_"+g_moduleName,val:"jsVal"};//,jscallback:"callback1"};
				var body = AwesomeMessageBody.create(payload);
				//print(body['jscallback']);
				var buffer = AwesomeMessageBody.encode(body).finish();
				message['messageBody'] = buffer;
			}
			var buffer = AwesomeMessage.encode(message).finish();
			Suloki_OperateCache(buffer, g_moduleName);
			}
		});
		if(!isNaN(resCode))
		{
		//if(resCode == 0)
		//	print(g_moduleName+' Suloki_RequestAsync ok');
		//else
		//	print(g_moduleName+' Suloki_RequestAsync error, ret:'+resCode);
		}
		return 0;
	}
	if(resMsg['businessId'] == 0 && resMsg['messageId'] == 8 && resMsg['sequenceNumber'] == 50)
	{
		var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
		var payload = {protocolVersion: "2.0.0.0",businessId:0,messageId:0,messageType:0,sequenceNumber:g_sequenceid++};
		var message = AwesomeMessage.create(payload);
		var buffer = AwesomeMessage.encode(message).finish();
		var resCode = Suloki_RequestDirectToIomodule('iomoduletest', buffer, 3000, g_moduleName);
		if(!isNaN(resCode))
		{
		//if(resCode == 0)
		//	print('Suloki_RequestDirectToIomodule ok');
		//else
		//	print('Suloki_RequestDirectToIomodule error');
		}
		else
		{
			var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
			var resMsg = AwesomeMessage.decode(resCode);
			if(resMsg['errorCode'] == 0)
			{
				{
				var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
				var payload = {protocolVersion: "2.0.0.0",businessId:4,messageId:1,messageType:0,sequenceNumber:g_sequenceid++};
				var message = AwesomeMessage.create(payload);
				{
				var AwesomeMessageBody = g_root.lookupType("suloki.SulokiCacheBody");
				var payload = {key:"50_"+g_moduleName,val:"jsVal"};//,jscallback:"callback1"};
				var body = AwesomeMessageBody.create(payload);
				//print(body['jscallback']);
				var buffer = AwesomeMessageBody.encode(body).finish();
				message['messageBody'] = buffer;
				}
				var buffer = AwesomeMessage.encode(message).finish();
				Suloki_OperateCache(buffer, g_moduleName);
				}
			}
			else
				print('Suloki_RequestDirectToIomodule error1');
		}
		return 0;
	}
	return 0;
	//
	//print(g_moduleName+' callbackOwner recv msg '+userdata+' from '+moduleNameFrom);
	//test response
	/*
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	var resMsg = AwesomeMessage.decode(msg);
	//
	if(resMsg['businessId'] == 3 && resMsg['messageId'] == 2)
	{
	print('timer');
	return 0;
	}
	if(resMsg['messageType'] == 0)
	{
		resMsg['messageType'] = 1;
		var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
		var buffer = AwesomeMessage.encode(resMsg).finish();
		var resMsgData = Suloki_Response(buffer, moduleNameFrom, 0, g_moduleName);
		return;
	}
	*/
	//test Request
	/*
	//if(moduleNameFrom == g_moduleName)
	{
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	//g_sequenceid = g_sequenceid+1;
	var payload = {protocolVersion: "2.0.0.0",businessId:0,messageId:0,messageType:0,sequenceNumber:g_sequenceid++};
	var message = AwesomeMessage.create(payload);
	var buffer = AwesomeMessage.encode(message).finish();
	print(g_moduleName+' request to iomoduletest');
	var resMsgData = Suloki_Request(buffer, 'iomoduletest', 0, g_moduleName, 3000);
	{
		var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
		var resMsg = AwesomeMessage.decode(resMsgData);
		var size = resMsg['routers'].length;
		print(g_moduleName+' recv response from '+resMsg['routers'][size-1]);
	}
	}
	*/
	//test request async
	/*
	{
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	//g_sequenceid = g_sequenceid+1;
	var payload = {protocolVersion: "2.0.0.0",businessId:0,messageId:0,messageType:0,sequenceNumber:g_sequenceid++};
	var message = AwesomeMessage.create(payload);
	var buffer = AwesomeMessage.encode(message).finish();
	print(g_moduleName+' requestasync to jsmodulehandler');
	var resCode = Suloki_RequestAsync(buffer, 'jsmodulehandler', 0, g_moduleName, 3000, 0, function(resData,userData,moduleFrom){
		print('response async userData:'+userData+' moduleFrom:'+moduleFrom);
	});
	if(!isNaN(resCode))
	{
	if(resCode == 0)
		print(g_moduleName+' Suloki_RequestAsync ok');
	else
		print(g_moduleName+' recvasync response, ret:'+resCode);
	}
	//print(g_moduleName+' recvasync response, ret:'+resCode);
	{
		//var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
		//var resMsg = AwesomeMessage.decode(resMsgData);
		//var size = resMsg['routers'].length;
		//print(g_moduleName+' recv response from '+resMsg['routers'][size-1]);
	}
	}
	*/
	return 0;
}

EventLoop.callbackBroadcast=function(msg, userdata, moduleNameFrom) {
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	var resMsg = AwesomeMessage.decode(msg);
	//test function and performance
	if(resMsg['businessId'] == 0 && resMsg['messageId'] == 8 && resMsg['sequenceNumber'] == 20)
	{
		{
		var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
		var payload = {protocolVersion: "2.0.0.0",businessId:4,messageId:1,messageType:0,sequenceNumber:g_sequenceid++};
		var message = AwesomeMessage.create(payload);
		{
			var AwesomeMessageBody = g_root.lookupType("suloki.SulokiCacheBody");
			var payload = {key:"20_"+g_moduleName,val:"jsVal"};//,jscallback:"callback1"};
			var body = AwesomeMessageBody.create(payload);
			//print(body['jscallback']);
			var buffer = AwesomeMessageBody.encode(body).finish();
			message['messageBody'] = buffer;
		}
		var buffer = AwesomeMessage.encode(message).finish();
		Suloki_OperateCache(buffer, g_moduleName);
		}
	}
	return 0;
	//print(g_moduleName+' callbackBroadcast recv msg '+userdata+' from '+moduleNameFrom);
	return 0;
}

EventLoop.callbackTaketurns=function(msg, userdata, moduleNameFrom) {
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	var resMsg = AwesomeMessage.decode(msg);
	//test function and performance
	if(resMsg['businessId'] == 0 && resMsg['messageId'] == 0 && resMsg['messageType'] == 2 && resMsg['sequenceNumber'] >= 1000000)
	{
		if(test_number == 0)
			test_timestart = new Date();
		test_number++;
		if((new Date())-test_timestart >= 1000*5 && test_print == 0)
		{
			test_print = 1;
			print(g_moduleName+' recv notice '+test_number/5+'/s');
		}
		return 0;
	}
	/*
	if(resMsg['businessId'] == 0 && resMsg['messageId'] == 8 && resMsg['sequenceNumber'] == 10)
	{
		{
		var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
		var payload = {protocolVersion: "2.0.0.0",businessId:4,messageId:1,messageType:0,sequenceNumber:g_sequenceid++};
		var message = AwesomeMessage.create(payload);
		{
			var AwesomeMessageBody = g_root.lookupType("suloki.SulokiCacheBody");
			var payload = {key:resMsg['sequenceNumber']+"_"+g_moduleName,val:"jsVal"};//,jscallback:"callback1"};
			var body = AwesomeMessageBody.create(payload);
			//print(body['jscallback']);
			var buffer = AwesomeMessageBody.encode(body).finish();
			message['messageBody'] = buffer;
		}
		var buffer = AwesomeMessage.encode(message).finish();
		Suloki_OperateCache(buffer, g_moduleName);
		}
	}
	*/
	return 0;
	//print(g_moduleName+' callbackTaketurns recv msg '+userdata+' from '+moduleNameFrom);
	//test Request
	//{
	//var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	////g_sequenceid = g_sequenceid+1;
	//var payload = {protocolVersion: "2.0.0.0",businessId:0,messageId:0,messageType:0,sequenceNumber:g_sequenceid++};
	//var message = AwesomeMessage.create(payload);
	//var buffer = AwesomeMessage.encode(message).finish();
	//print(g_moduleName+' request to iomoduletest');
	//var resMsgData = Suloki_Request(buffer, 'iomoduletest', 0, g_moduleName, 3000000);
	//{
	//	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	//	var resMsg = AwesomeMessage.decode(resMsgData);
	//	var size = resMsg['routers'].length;
	//	print(g_moduleName+' recv response from '+resMsg['routers'][size-1]);
	//}
	//}
	return 0;
}

EventLoop.initHandler=function(initData, moduleName) {
	g_moduleName = moduleName;
	{
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	var initMsg = AwesomeMessage.decode(initData);
	var body = initMsg['messageBody'];
	var AwesomeMessageBody = g_root.lookupType("suloki.SulokiInitMsgBody");
	var messageBody = AwesomeMessageBody.decode(body);
	print(g_moduleName + ' initHandler:'+messageBody['infomation']);
	}
	//
	{
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	//g_sequenceid = g_sequenceid+1;
	var payload = {protocolVersion: "2.0.0.0",businessId:1,messageId:0,messageType:0,sequenceNumber:g_sequenceid++};
	var message = AwesomeMessage.create(payload);
	{
		var AwesomeMessageBody = g_root.lookupType("suloki.SulokiRegRecvEventBody");
		var payload = {patternKey:g_moduleName,businessId:-1,messageId:-1,mode:1,right:0,policy:0,callback:"callbackOwner",userdata:0};//,jscallback:"callback1"};
		var body = AwesomeMessageBody.create(payload);
		//print(body['jscallback']);
		var buffer = AwesomeMessageBody.encode(body).finish();
		message['messageBody'] = buffer;
	}
	var buffer = AwesomeMessage.encode(message).finish();
	if(Suloki_RegRecvEventPattern(buffer, g_moduleName)!=0)
		print('Suloki_RegRecvEventPattern error');
	}
	//
	{
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	//g_sequenceid = g_sequenceid+1;
	var payload = {protocolVersion: "2.0.0.0",businessId:1,messageId:0,messageType:0,sequenceNumber:g_sequenceid++};
	var message = AwesomeMessage.create(payload);
	{
		var AwesomeMessageBody = g_root.lookupType("suloki.SulokiRegRecvEventBody");
		var payload = {patternKey:"broadcastkey",businessId:-1,messageId:-1,mode:1,right:1,policy:0,callback:"callbackBroadcast",userdata:1};//,jscallback:"callback1"};
		var body = AwesomeMessageBody.create(payload);
		//print(body['jscallback']);
		var buffer = AwesomeMessageBody.encode(body).finish();
		message['messageBody'] = buffer;
	}
	var buffer = AwesomeMessage.encode(message).finish();
	if(Suloki_RegRecvEventPattern(buffer, g_moduleName)!=0)
		print('Suloki_RegRecvEventPattern error');
	}
	//
	{
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	//g_sequenceid = g_sequenceid+1;
	var payload = {protocolVersion: "2.0.0.0",businessId:1,messageId:0,messageType:0,sequenceNumber:g_sequenceid++};
	var message = AwesomeMessage.create(payload);
	{
		var AwesomeMessageBody = g_root.lookupType("suloki.SulokiRegRecvEventBody");
		var payload = {patternKey:"taketurnskey",businessId:-1,messageId:-1,mode:1,right:1,policy:1,callback:"callbackTaketurns",userdata:2};//,jscallback:"callback1"};
		var body = AwesomeMessageBody.create(payload);
		//print(body['jscallback']);
		var buffer = AwesomeMessageBody.encode(body).finish();
		message['messageBody'] = buffer;
	}
	var buffer = AwesomeMessage.encode(message).finish();
	if(Suloki_RegRecvEventPattern(buffer, g_moduleName)!=0)
		print('Suloki_RegRecvEventPattern error');
	}
	//set timer
	/*
	{
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	var payload = {protocolVersion: "2.0.0.0",businessId:3,messageId:0,messageType:0,sequenceNumber:g_sequenceid++};
	var message = AwesomeMessage.create(payload);
	{
		var AwesomeMessageBody = g_root.lookupType("suloki.SulokiSetTimerBody");
		var payload = {id:0,patternKey:g_moduleName,timeout:1000};//,jscallback:"callback1"};
		var body = AwesomeMessageBody.create(payload);
		//print(body['jscallback']);
		var buffer = AwesomeMessageBody.encode(body).finish();
		message['messageBody'] = buffer;
	}
	var buffer = AwesomeMessage.encode(message).finish();
	var resCode = 1;//Suloki_RequestDirectToIomodule("timer",buffer,3000, g_moduleName);
	if(!isNaN(resCode))
	{
	if(resCode == 0)
		print('Suloki_RequestDirectToIomodule set timer ok');
	else
		print('Suloki_RequestDirectToIomodule set timer error');
	}
	else
	{
		var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
		var resMsg = AwesomeMessage.decode(resCode);
		if(resMsg['errorCode'] == 0)
			print('Suloki_RequestDirectToIomodule set timer ok');
		else
			print('Suloki_RequestDirectToIomodule set timer error');
	}
	}
	*/
	//cache
	/*
	{
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	var payload = {protocolVersion: "2.0.0.0",businessId:4,messageId:1,messageType:0,sequenceNumber:g_sequenceid++};
	var message = AwesomeMessage.create(payload);
	{
		var AwesomeMessageBody = g_root.lookupType("suloki.SulokiCacheBody");
		var payload = {key:"jsKey",val:"jsVal"};//,jscallback:"callback1"};
		var body = AwesomeMessageBody.create(payload);
		//print(body['jscallback']);
		var buffer = AwesomeMessageBody.encode(body).finish();
		message['messageBody'] = buffer;
	}
	var buffer = AwesomeMessage.encode(message).finish();
	var resCode = Suloki_OperateCache(buffer, g_moduleName);
	if(!isNaN(resCode))
	{
	if(resCode == 0)
		print('Suloki_OperateCache set ok1');
	else
		print('Suloki_OperateCache set error');
	}
	else
	{
		var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
		var resMsg = AwesomeMessage.decode(resCode);
		if(resMsg['errorCode'] == 0)
			print('Suloki_OperateCache set ok2');
		else
			print('Suloki_OperateCache set error');
	}
	}
	{
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	var payload = {protocolVersion: "2.0.0.0",businessId:4,messageId:0,messageType:0,sequenceNumber:g_sequenceid++};
	var message = AwesomeMessage.create(payload);
	{
		var AwesomeMessageBody = g_root.lookupType("suloki.SulokiCacheBody");
		var payload = {key:"jsKey",val:""};//,jscallback:"callback1"};
		var body = AwesomeMessageBody.create(payload);
		//print(body['jscallback']);
		var buffer = AwesomeMessageBody.encode(body).finish();
		message['messageBody'] = buffer;
	}
	var buffer = AwesomeMessage.encode(message).finish();
	var resCode = Suloki_OperateCache(buffer, g_moduleName);
	if(!isNaN(resCode))
	{
	if(resCode == 0)
		print('Suloki_OperateCache get ok1');
	else
		print('Suloki_OperateCache get error');
	}
	else
	{
		var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
		var resMsg = AwesomeMessage.decode(resCode);
		if(resMsg['errorCode'] == 0)
		{
			var AwesomeMessage = g_root.lookupType("suloki.SulokiCacheBody");
			var resbody = AwesomeMessage.decode(resMsg['messageBody']);
			print('Suloki_OperateCache get ok2 '+ resbody['val']);
		}
		else
			print('Suloki_OperateCache get error');
	}
	}
	*/
	log(g_moduleName+' initHandler ok', 5);
	return 0;
}
EventLoop.startHandler=function(startData) {
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	var startMsg = AwesomeMessage.decode(startData);
	var body = startMsg['messageBody'];
	var AwesomeMessageBody = g_root.lookupType("suloki.SulokiStartMsgBody");
	var messageBody = AwesomeMessageBody.decode(body);
	print(g_moduleName + ' startHandler:'+messageBody['infomation']);
	//
	{
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	//g_sequenceid = g_sequenceid+1;
	var payload = {protocolVersion: "2.0.0.0",businessId:0,messageId:0,messageType:2,sequenceNumber:g_sequenceid++};
	var message = AwesomeMessage.create(payload);
	var buffer = AwesomeMessage.encode(message).finish();
	//if(Suloki_Notify(buffer, "taketurnskey", 0, g_moduleName)!=0)
	//	print('Suloki_Notify error');
	//if(Suloki_Notify(buffer, "taketurnskey", 0, g_moduleName)!=0)
	//	print('Suloki_Notify error');
	//if(Suloki_Notify(buffer, "taketurnskey", 0, g_moduleName)!=0)
	//	print('Suloki_Notify error');
	//if(Suloki_Notify(buffer, "jsmodulehandler", 0, g_moduleName)!=0)
	//	print('Suloki_Notify error');
	//if(Suloki_Notify(buffer, "iomoduletest", 0, g_moduleName)!=0)
	//	print('Suloki_Notify error');
	//if(Suloki_Notify(buffer, "cppmodulehandler", 0, g_moduleName)!=0)
	//	print('Suloki_Notify error');
	//if(Suloki_Notify(buffer, "testkey", 0, g_moduleName)!=0)
	//	print('Suloki_Notify error');
	}
	//
	log(g_moduleName+' startHandler ok', 5);
	return 0;
}
EventLoop.stopHandler=function(stopData) {
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	var stopMsg = AwesomeMessage.decode(stopData);
	var body = stopMsg['messageBody'];
	var AwesomeMessageBody = g_root.lookupType("suloki.SulokiStopMsgBody");
	var messageBody = AwesomeMessageBody.decode(body);
	print(g_moduleName + ' stopHandler:'+messageBody['infomation']);
	return 0;
}
EventLoop.endHandler=function(endData) {
	//cancel timer
	/*
	{
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	var payload = {protocolVersion: "2.0.0.0",businessId:3,messageId:1,messageType:0,sequenceNumber:g_sequenceid++};
	var message = AwesomeMessage.create(payload);
	{
		var AwesomeMessageBody = g_root.lookupType("suloki.SulokiSetTimerBody");
		var payload = {id:0,patternKey:g_moduleName,timeout:1000};//,jscallback:"callback1"};
		var body = AwesomeMessageBody.create(payload);
		//print(body['jscallback']);
		var buffer = AwesomeMessageBody.encode(body).finish();
		message['messageBody'] = buffer;
	}
	var buffer = AwesomeMessage.encode(message).finish();
	if(Suloki_RequestDirectToIomodule("timer",buffer,3000, g_moduleName)!=0)
		print('Suloki_RequestDirectToIomodule cancel timer error');
	else
		print('Suloki_RequestDirectToIomodule cancel timer ok');
	}
	*/
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	var endMsg = AwesomeMessage.decode(endData);
	var body = endMsg['messageBody'];
	var AwesomeMessageBody = g_root.lookupType("suloki.SulokiClearMsgBody");
	var messageBody = AwesomeMessageBody.decode(body);
	print(g_moduleName + ' endHandler:'+messageBody['infomation']);
	return 0;
}

EventLoop.fdPollHandler = function(revents, moduleNameFrom) {//function(fd, revents) {
	//print('recv event:'+fd);
	var AwesomeMessage = g_root.lookupType("suloki.SulokiMessage");
	var message = AwesomeMessage.decode(revents);
	log(g_moduleName+' proto from c++ to js:'+message['protocolVersion']+' from:'+moduleNameFrom, 5);
}

function setTimeout(func, delay) {
    var cb_func;
    var bind_args;
    var timer_id;

    if (typeof delay !== 'number') {
        throw new TypeError('delay is not a number');
    }

    if (typeof func === 'string') {
        // Legacy case: callback is a string.
        cb_func = eval.bind(this, func);
    } else if (typeof func !== 'function') {
        throw new TypeError('callback is not a function/string');
    } else if (arguments.length > 2) {
        // Special case: callback arguments are provided.
        bind_args = Array.prototype.slice.call(arguments, 2);  // [ arg1, arg2, ... ]
        bind_args.unshift(this);  // [ global(this), arg1, arg2, ... ]
        cb_func = func.bind.apply(func, bind_args);
    } else {
        // Normal case: callback given as a function without arguments.
        cb_func = func;
    }

    timer_id = EventLoop.createTimer(cb_func, delay, true /*oneshot*/);

    return timer_id;
}

function clearTimeout(timer_id) {
    if (typeof timer_id !== 'number') {
        throw new TypeError('timer ID is not a number');
    }
    var success = EventLoop.deleteTimer(timer_id);  /* retval ignored */
}

function setInterval(func, delay) {
    var cb_func;
    var bind_args;
    var timer_id;

    if (typeof delay !== 'number') {
        throw new TypeError('delay is not a number');
    }

    if (typeof func === 'string') {
        // Legacy case: callback is a string.
        cb_func = eval.bind(this, func);
    } else if (typeof func !== 'function') {
        throw new TypeError('callback is not a function/string');
    } else if (arguments.length > 2) {
        // Special case: callback arguments are provided.
        bind_args = Array.prototype.slice.call(arguments, 2);  // [ arg1, arg2, ... ]
        bind_args.unshift(this);  // [ global(this), arg1, arg2, ... ]
        cb_func = func.bind.apply(func, bind_args);
    } else {
        // Normal case: callback given as a function without arguments.
        cb_func = func;
    }

    timer_id = EventLoop.createTimer(cb_func, delay, false /*oneshot*/);

    return timer_id;
}

function clearInterval(timer_id) {
    if (typeof timer_id !== 'number') {
        throw new TypeError('timer ID is not a number');
    }
    EventLoop.deleteTimer(timer_id);
}

function requestEventLoopExit() {
    EventLoop.requestExit();
}

/*
 *  Socket handling
 *
 *  Ideally this would be implemented more in C than here for more speed
 *  and smaller footprint: C code would directly maintain the callback state
 *  and such.
 *
 *  Also for more optimal I/O, the buffer churn caused by allocating and
 *  freeing a lot of buffer values could be eliminated by reusing buffers.
 *  Socket reads would then go into a pre-allocated buffer, for instance.
 */

EventLoop.socketListening = {};
EventLoop.socketReading = {};
EventLoop.socketConnecting = {};

//EventLoop.fdPollHandler = function(fd, revents) {
//	
/*
    var data;
    var cb;
    var rc;
    var acc_res;
	
	print('fdPollHandler');
	print(fd);
	print(revents);
	return;

    //print('activity on fd', fd, 'revents', revents);

    if (revents & Poll.POLLIN) {
        cb = this.socketReading[fd];
        if (cb) {
            data = Socket.read(fd);  // no size control now
            //print('READ', Duktape.enc('jx', data));
            if (data.length === 0) {
                this.close(fd);
                return;
            }
            cb(fd, data);
        } else {
            cb = this.socketListening[fd];
            if (cb) {
                acc_res = Socket.accept(fd);
                //print('ACCEPT:', Duktape.enc('jx', acc_res));
                cb(acc_res.fd, acc_res.addr, acc_res.port);
            } else {
                //print('UNKNOWN');
            }
        }
    }

    if (revents & Poll.POLLOUT) {
        // Connected
        cb = this.socketConnecting[fd];
        if (cb) {
            delete this.socketConnecting[fd];
            cb(fd);
        }
    }

    if ((revents & ~(Poll.POLLIN | Poll.POLLOUT)) !== 0) {
        //print('unexpected revents, close fd');
        this.close(fd);
    }
*/
//}

EventLoop.server = function(address, port, cb_accepted) {
    var fd = Socket.createServerSocket(address, port);
    this.socketListening[fd] = cb_accepted;
    this.listenFd(fd, Poll.POLLIN);
}

EventLoop.connect = function(address, port, cb_connected) {
    var fd = Socket.connect(address, port);
    this.socketConnecting[fd] = cb_connected;
    this.listenFd(fd, Poll.POLLOUT);
}

EventLoop.close = function(fd) {
    EventLoop.listenFd(fd, 0);
    delete this.socketListening[fd];
    delete this.socketReading[fd];
    delete this.socketConnecting[fd];
    Socket.close(fd);
}

EventLoop.setReader = function(fd, cb_read) {
    this.socketReading[fd] = cb_read;
    this.listenFd(fd, Poll.POLLIN);
}

EventLoop.write = function(fd, data) {
    // This simple example doesn't have support for write blocking / draining
    var rc = Socket.write(fd, Duktape.Buffer(data));
}
