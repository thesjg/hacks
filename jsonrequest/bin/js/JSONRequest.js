JSONRequest = {

	SWFPath: '../bin/JSONRequest.swf',
	
	_initialized: false,
	_SWFHandle: null,

	_requestNumber: 0,
	_requestQueue: Array(),
	_callbacks: Array(),

	post: function(url, send, done, timeout)
	{	
		if (timeout == undefined)
			timeout = 10000;
	
		var myRequestNumber = this._requestNumber;
		this._requestNumber++;

		this._callbacks[myRequestNumber] = done;
		if (this._SWFHandle)
			this._SWFHandle.post(myRequestNumber, url, send, timeout);
		else {
			var request = Object();
			request.method = 'POST';
			request.number = myRequestNumber;
			request.url = url;
			request.send = send;
			request.timeout = timeout;
			this._requestQueue.push(request);
		}

		return myRequestNumber;
	},
	
	get: function(url, done, timeout)
	{
		if (timeout == undefined)
			timeout = 10000;
	
		var myRequestNumber = this._requestNumber;
		this._requestNumber++;

		this._callbacks[myRequestNumber] = done;
		if (this._SWFHandle)
  		    this._SWFHandle.get(myRequestNumber, url, timeout);
		else {
			var request = Object();
			request.method = 'GET';
			request.number = myRequestNumber;
			request.url = url;
			request.timeout = timeout;
			this._requestQueue.push(request);
		}

		return myRequestNumber;
	},
	
	cancel: function()
	{
	},
	
	_initialize: function()
	{	
		var container = document.createElement('div');
		container.setAttribute('id', 'JSONRequestFlashContainer');
		document.body.appendChild(container);
		var params = {
			allowScriptAccess: "always"
		};
		swfobject.embedSWF(this.SWFPath, 'JSONRequestFlashContainer',
						   '100%', '100%', '9.0.0', 'expressInstall.swf',
						   {}, params);
	},
	
	_SWFInitialized: function()
	{
	    this._SWFHandle = document.getElementById('JSONRequestFlashContainer');
	    this._initialized = true;
		
		while (request = this._requestQueue.pop()) {
			if (request.method == 'POST')
				this._SWFHandle.post(request.number, request.url,
				                     request.send, request.timeout);
			else
				this._SWFHandle.get(request.number, request.url,
				                    request.timeout);
		}
	},
	
	_requestComplete: function(requestNumber, data, exception)
	{
		this._callbacks[requestNumber].call(this, requestNumber, data, exception);
	}
}

var __JSONRequestOnload = window.onload;
window.onload = function() {
    JSONRequest._initialize();
    if (__JSONRequestOnload)
		__JSONRequestOnload();
}