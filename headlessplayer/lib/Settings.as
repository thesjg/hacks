package  
{
	import flash.net.SharedObject;

	public class Settings
	{
		private var _settings:SharedObject;

		public function Settings() 
		{
			_settings = SharedObject.getLocal('HPlayer');
		}
		
		public function get volume():Number
		{
			return _settings.data.volume || 1;
		}

		public function set volume(volume:Number):void
		{
			_settings.data.volume = volume;
		}
	}
}