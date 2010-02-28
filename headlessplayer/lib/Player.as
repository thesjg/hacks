package  
{
	import flash.external.ExternalInterface;
	import flash.net.URLRequest;
	import flash.system.Security;

	import flash.media.Sound;
	import flash.media.SoundChannel;
	import flash.media.SoundLoaderContext;
	import flash.media.SoundTransform;

	import mx.core.Application;

	import PlayerInfo;
	import PlayerNotify;
	import Settings;

	public class Player
	{
		private var _playerInfo:PlayerInfo;
		private var _playerNotify:PlayerNotify;
		private var _settings:Settings;

		private var _sound:Sound;
		private var _channel:SoundChannel;
		private var _transform:SoundTransform;

		private var _paused:Boolean;
		private var _pauseOffset:Number;

		public function Player() 
		{
			_sound = new Sound();
			_channel = new SoundChannel();
			_transform = new SoundTransform();
			_settings = new Settings();

			_playerInfo = new PlayerInfo(this);
			_playerNotify = new PlayerNotify(this);

			_setVolume(_settings.volume);

			_paused = false;
			_pauseOffset = 0;
		}

		public function get sound():Sound
		{
			return (_sound);
		}

		public function get channel():SoundChannel
		{
			return (_channel);
		}

		public function get transform():SoundTransform
		{
			return (_transform);
		}

		public function get paused():Boolean
		{
			return (_paused);
		}

		public function get pauseOffset():Number
		{
			return (_pauseOffset);
		}

		public function exportInterfaces():Boolean
		{
			_playerInfo.exportInterfaces();

			if (ExternalInterface.available) {
				ExternalInterface.addCallback('loadFile', this.loadFile);
				ExternalInterface.addCallback('play', this.play);
				ExternalInterface.addCallback('pause', this.pause);
				ExternalInterface.addCallback('setVolume', this.setVolume);
				ExternalInterface.addCallback('stop', this.stop);

				Security.allowDomain('*')
			} else {
				return (false);
			}
			
			return (true);
		}

		public function registerCallbacks(callbacks:Array):Boolean
		{
			_playerNotify.registerCallbacks(callbacks);

			ExternalInterface.call(callbacks['object'] + '.' + callbacks['init']);

			return (true);
		}

		public function loadFile(url:String):void
		{
			_sound.load(new URLRequest(url));
		}

		public function play():void
		{
			if (_paused) {
				_channel = _sound.play(_pauseOffset);
				_paused = false;
				_pauseOffset = 0;
			} else {
				_channel = _sound.play();
			}

			_channel.soundTransform = _transform;
		}

		public function pause():void
		{
			_paused = true;
			_pauseOffset = _channel.position;
			_channel.stop();
		}

		private function _setVolume(volume:Number):void
		{
			_transform.volume = volume;
			_channel.soundTransform = _transform;
		}

		public function setVolume(volume:Number):void
		{
			_settings.volume = volume;
			_setVolume(volume);
		}

		public function stop():void
		{
			_channel.stop();
		}
	}
}