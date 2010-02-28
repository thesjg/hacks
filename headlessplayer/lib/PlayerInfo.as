package  
{
	import flash.external.ExternalInterface;

	import Player;

	public class PlayerInfo
	{
		private var _player:Player;

		public function PlayerInfo(player:Player) 
		{
			_player = player;
		}

		public function exportInterfaces():Boolean
		{
			if (ExternalInterface.available) {
				ExternalInterface.addCallback('isPaused', this.isPaused);
				ExternalInterface.addCallback('getPauseOffset', this.getPauseOffset);
				ExternalInterface.addCallback('getUrl', this.getUrl);
				ExternalInterface.addCallback('getDuration', this.getDuration);
				ExternalInterface.addCallback('getPosition', this.getPosition);
				ExternalInterface.addCallback('getBytesLoaded', this.getBytesLoaded);
				ExternalInterface.addCallback('getBytesTotal', this.getBytesTotal);
				ExternalInterface.addCallback('getVolume', this.getVolume);
				ExternalInterface.addCallback('getId3', this.getId3);
			} else {
				return (false);
			}
			
			return (true);
		}

		public function isPaused():Boolean
		{
			return (_player.paused);
		}

		public function getPauseOffset():Number
		{
			return (_player.pauseOffset);
		}

		public function getUrl():String
		{
			return (_player.sound.url);
		}

		public function getDuration():Number
		{
			return (_player.sound.length / 1000);
		}

		public function getPosition():Number
		{
			return (_player.channel.position);
		}

		public function getBytesLoaded():Number
		{
			return (_player.sound.bytesLoaded);
		}

		public function getBytesTotal():Number
		{
			return (_player.sound.bytesTotal);
		}

		public function getVolume():Number
		{
			return (_player.transform.volume);
		}

		public function getId3():Object
		{
			return (_player.sound.id3);
		}
	}
}