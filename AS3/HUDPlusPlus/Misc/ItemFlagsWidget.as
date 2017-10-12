package  {
	
	import flash.display.MovieClip;
	
	public class ItemIconsWidget extends MovieClip 
	{
		
		public var BookReadIcon_mc : MovieClip;
		
		public var QuestItemIcon_mc : MovieClip;
		
		public var BookSkillIcon_mc : MovieClip;

		
		private var _canDisplay : Boolean = true;

				//_canDisplay
		public function get canDisplay() : Boolean
		{
			return this._canDisplay;
		}

		public function set canDisplay(param1 : Boolean) : *
		{
			this._canDisplay = param1;
		}	
		
		public function ItemIconsWidget() 
		{
			super();
			BookReadIcon_mc.visible = false;
			QuestItemIcon_mc.visible = false;
			BookSkillIcon_mc.visible = false;
		}
		
		public function PositionElements(showReadIcon: Boolean, showQuestIcon: Boolean, showSkillBookIcon: Boolean): void
		{
		 	var curX: Number = 0;
			if(showQuestIcon)
			{
				QuestItemIcon_mc.x = curX;
				QuestItemIcon_mc.visible = true;
				curX += 20;
			}
			else
			{
				QuestItemIcon_mc.visible = false;
			}
		 	if(showReadIcon)
			{
				BookReadIcon_mc.x = curX;
				BookReadIcon_mc.visible = true;
				curX += 20;
			}
			else
			{
				BookReadIcon_mc.visible = false;
			}
		 	if(showSkillBookIcon)
			{
				BookSkillIcon_mc.x = curX;
				BookSkillIcon_mc.visible = true;
				curX += 20;
			}
			else
			{
				BookSkillIcon_mc.visible = false;
			}
		}
	}
}
