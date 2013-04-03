-- Get url
CCHttpRequest:open("https://google.com/search?q=LuaProxy&safe=strict", kHttpGet):sendWithHandler(
function(res, hnd)
	print(res:getResponseData(), res:getResponseCode())
end)

-- Post url
CCHttpRequest:open("https://google.com/search?q=LuaProxy&safe=strict", kHttpPost, "query=param1&other=params"):sendWithHandler(
function(res, hnd)
	print(res:getResponseData(), res:getResponseCode())
end)
