/*
  Copyright (c) 2009, Samuel J. Greear
  All rights reserved.

  Redistribution and use in source and binary forms, with or without 
  modification, are permitted provided that the following conditions are
  met:

  * Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
  
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the 
    documentation and/or other materials provided with the distribution.
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package  
{
	import flash.external.ExternalInterface;
	import flash.system.Security;
	import flash.utils.Timer;
	
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.net.URLRequestHeader;
	import flash.net.URLRequestMethod;

	import flash.events.Event;
	import flash.events.TimerEvent;
	import flash.events.ProgressEvent;
	import flash.events.SecurityErrorEvent;
	import flash.events.HTTPStatusEvent;
	import flash.events.IOErrorEvent;
	
	import JSONDecoder;
	import JSONParseError;


	public class JSONRequest 
	{
		private var _exception:Object;
		private var _delay:Number;
		
		public function JSONRequest() 
		{
			_exception = new Object();
			_exception.name = 'JSONRequestError';
			_delay = 0;
		}
		
		public function exportInterfaces():Boolean
		{
			if (ExternalInterface.available) {
				ExternalInterface.addCallback('post', this.post);
				ExternalInterface.addCallback('get', this.get);
				ExternalInterface.addCallback('cancel', this.cancel);
			
				Security.allowDomain('*')
			
				ExternalInterface.call('JSONRequest._SWFInitialized');
			} else {
				return false;
			}
			
			return true;
		}

		public function post(requestNumber:Number, url:String, send:Object,
						     timeout:Number):Boolean
		{	
			var request:URLRequest = new URLRequest(url);
			request.data = JSON.encode(send);
			request.method = URLRequestMethod.POST;
			_setupRequestHeaders(request);

			_initiateRequest(requestNumber, request);
			
			return true;
		}

		public function get(requestNumber:Number, url:String,
						    timeout:Number):Boolean
		{
			var request:URLRequest = new URLRequest(url);
			request.method = URLRequestMethod.GET;
			_setupRequestHeaders(request);

			_initiateRequest(requestNumber, request);
			
			return true;
		}
		
		public function cancel():void
		{
			_delay += 10;
		}
		
		private function _setupRequestHeaders(request:URLRequest):void
		{
			request.requestHeaders =
				new Array(new URLRequestHeader('Content-Type', 'application/jsonrequest'),
						  new URLRequestHeader('Accept', 'application/jsonrequest'),
						  new URLRequestHeader('Origin', 'null')); /* XXX: Implement Origin header */
		}
		
		private function _setupEventListeners(loader:URLLoader, requestNumber:Number):void
		{
			loader.addEventListener(Event.COMPLETE,
				function(event:Event):void {
					var loader:URLLoader = URLLoader(event.target);
					_returnResult(requestNumber, loader.data);
				});
			
			loader.addEventListener(SecurityErrorEvent.SECURITY_ERROR,
				function(event:SecurityErrorEvent):void {
					_delay += 500 + 
					_returnException(requestNumber, 'flash security error');
				});
			
			loader.addEventListener(IOErrorEvent.IO_ERROR,
				function():void {
					_returnException(requestNumber, 'no response');
				});
		}
		
		private function _initiateRequest(requestNumber:Number, request:URLRequest):void
		{
			var loader:URLLoader = new URLLoader();
			_setupEventListeners(loader, requestNumber);
			
			var timer:Timer = new Timer(_delay, 1);
			timer.addEventListener(TimerEvent.TIMER,
				function():void {
					loader.load(request);
				});
			timer.start();
		}
		
		private function _returnResult(requestNumber:Number, data:String):void
		{
			var returnData:Object;

			try {
				returnData = JSON.decode(data, false);
			} catch (exception:JSONParseError) {
				return _returnException(requestNumber, 'bad response');
			}

			_delay -= 10;
			if (_delay < 0) _delay = 0;
			
			ExternalInterface.call('JSONRequest._requestComplete', requestNumber,
			                       data, null);
		}
		
		private function _returnException(requestNumber:Number, message:String):void
		{
			var exception:Object = _exception;
			exception.message = message;

			_delay += Math.floor(Math.random() * 511);

			ExternalInterface.call('JSONRequest._requestComplete', requestNumber,
							       null, exception);
		}
	}
}