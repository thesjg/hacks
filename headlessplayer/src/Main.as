package 
{
	import flash.display.Sprite;
	import flash.events.Event;

	import Player;

	/**
	 * ...
	 * @author Samuel J. Greear
	 */
	public class Main extends Sprite 
	{
		private var _player:Player;

		public function Main():void 
		{
			if (stage) _init();
			else addEventListener(Event.ADDED_TO_STAGE, _init);
		}
		
		private function _init(e:Event = null):void 
		{
			var callbacks:Array = new Array();
			var params:Object = loaderInfo.parameters;

			removeEventListener(Event.ADDED_TO_STAGE, init);

			callbacks['object'] = params.cbObject || 'HPlayer';
			callbacks['init'] = params.cbInit || '_SWFInitialized';
			callbacks['open'] = params.cbOpen || '_onOpen';
			callbacks['complete'] = params.cbComplete || ' _onComplete';
			callbacks['id3'] = params.cbId3 || '_onId3';
			callbacks['error'] = params.cbError || '_onError';
			callbacks['progress'] = params.cbProgress || '_onProgress';

			_player = new Player();
			_player.exportInterfaces();
			_player.registerCallbacks(callbacks);

//			_test();
		}

		private function _test():void
		{
			_player.loadFile('test.mp3');
			_player.play();
			_player.stop();
			_player.play();
			_player.pause();
			_player.play();
			_player.setVolume(0.2);
		}
	}
}