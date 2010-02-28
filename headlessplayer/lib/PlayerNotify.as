package  
{
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.external.ExternalInterface;
	import flash.media.Sound;

	import Player;

	public class PlayerNotify
	{
		private var _player:Player;
		private var _callbacks:Array;

		public function PlayerNotify(player:Player) 
		{
			_player = player;
			
			_player.sound.addEventListener(Event.OPEN, this._onSoundOpen);
			_player.sound.addEventListener(Event.COMPLETE, this._onSoundComplete);
			_player.sound.addEventListener(Event.ID3, this._onSoundId3);
			_player.sound.addEventListener(IOErrorEvent.IO_ERROR, this._onSoundIOError);
			_player.sound.addEventListener(ProgressEvent.PROGRESS, this._onSoundProgress);
		}

		public function registerCallbacks(callbacks:Array):void
		{
			_callbacks = callbacks;
		}

		private function _onSoundOpen(ev:Event):void
		{
			ExternalInterface.call(_callbacks['object'] + '.' + _callbacks['open']);
		}

		private function _onSoundComplete(ev:Event):void
		{
			ExternalInterface.call(_callbacks['object'] + '.' + _callbacks['complete']);
		}

		private function _onSoundId3(ev:Event):void
		{
			ExternalInterface.call(_callbacks['object'] + '.' + _callbacks['id3']);
		}

		private function _onSoundIOError(ev:IOErrorEvent):void
		{
			ExternalInterface.call(_callbacks['object'] + '.' + _callbacks['error']);
		}

		private function _onSoundProgress(ev:ProgressEvent):void
		{
			ExternalInterface.call(_callbacks['object'] + '.' + _callbacks['progress']);
		}
	}
}