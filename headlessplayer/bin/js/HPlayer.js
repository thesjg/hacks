HPlayer = {

    SWFPath: 'HPlayer.swf',

	_initialized: false,
	_SWFHandle: null,
	_play: null,
	_volume: null,
	_paused: false,

	play: function(url)
	{
		if (this._SWFHandle) {
			if (this._paused == true)
				this._SWFHandle.play();
			else {
				this._SWFHandle.loadFile(url);
				this._SWFHandle.play()
			}
		}

		this._play = url;
	},

	pause: function()
	{
		if (this._SWFHandle) {
			this._SWFHandle.pause();
			this._paused = true;
		}
	},

	stop: function()
	{
		if (this._SWFHandle)
			this._SWFHandle.stop();
	},

	mute: function()
	{
		if (this._SWFHandle) {
			this._volume = this._SWFHandle.getVolume();
			this._SWFHandle.setVolume(0);
		}
	},

	unmute: function()
	{
		this.setVolume(this._volume);
	},

	setVolume: function(volume)
	{
		if (this._SWFHandle)
			this._SWFHandle.setVolume(volume);
	},

	_initialize: function()
	{
		var container = document.createElement('div');
		container.setAttribute('id', 'HPlayerFlashContainer');
		document.body.appendChild(container);
		var params = {
			allowScriptAccess: 'always'
		};
		swfobject.embedSWF(this.SWFPath, 'HPlayerFlashContainer', '100%', '100%', '9.0.0', 'expressInstall.swf', {}, params);
	},

	_SWFInitialized: function()
	{
		this._SWFHandle = document.getElementById('HPlayerFlashContainer');
		this._initialized = true;

		if (this._play != null)
			this.play(this._play);

//		alert('_SWFInitialized');
	},

	_onOpen: function()
	{
//		alert('_onOpen');
	},

	_onComplete: function()
	{
//		alert('_onComplete');
	},

	_onId3: function()
	{
//		alert('_onId3');
	},

	_onError: function()
	{
		alert('_onError');
	},

	_onProgress: function()
	{
		//alert('_onProgress');
	}
}

var __HPlayerOnload = window.onload;
window.onload = function() {
	HPlayer._initialize();
	if (__HPlayerOnload)
		__HPlayerOnload();
}