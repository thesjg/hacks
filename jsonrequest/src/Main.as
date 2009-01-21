package 
{
	import flash.display.Sprite;
	import flash.events.Event;

	import JSONRequest;

	public class Main extends Sprite 
	{
		private var _JSR:JSONRequest;
		
		public function Main():void 
		{
			if (stage)
				init();
			else
				addEventListener(Event.ADDED_TO_STAGE, init);
		}
		
		private function init(e:Event = null):void 
		{
			removeEventListener(Event.ADDED_TO_STAGE, init);

			_JSR = new JSONRequest();
			_JSR.exportInterfaces();
		}
	}
}