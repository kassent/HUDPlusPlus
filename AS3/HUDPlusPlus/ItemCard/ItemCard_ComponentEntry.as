package
{
	import flash.display.MovieClip;
	import flash.geom.Rectangle;

	public class ItemCard_ComponentEntry extends ItemCard_StandardEntry
	{
		public var FavIcon_mc : MovieClip;

		public function ItemCard_ComponentEntry()
		{
			super();
		}

		override public function PopulateEntry(param1:Object) : *
		{
			super.PopulateEntry(param1);
			if(param1.isTagged)
			{
				FavIcon_mc.x = ItemCardLabel_tf.x + ItemCardLabel_tf.getLineMetrics(0).x + ItemCardLabel_tf.textWidth + 15;
				FavIcon_mc.visible = true;
			}
			else
			{
				FavIcon_mc.visible = false;
			}
		}
	}
}