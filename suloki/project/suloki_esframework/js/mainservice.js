function handle(message, context)
{
	print('Hello world!!'+message+context);
	res = Suloki_OperateCache('{\"add\":\"data\"}');
	print('mainservice.js:'+res);
}
