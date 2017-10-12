package
{
	import flash.display.MovieClip;
	import flash.text.TextFieldAutoSize;
	import scaleform.gfx.TextFieldEx;
	import flash.text.TextField;

	public class ItemCard_DescriptionEntry extends ItemCard_StandardEntry
	{


		public var Background_mc : MovieClip;

		public function ItemCard_DescriptionEntry()
		{
			super();
			TextFieldEx.setTextAutoSize(ItemCardLabel_tf,TextFieldEx.TEXTAUTOSZ_NONE);
			ItemCardLabel_tf.autoSize = TextFieldAutoSize.LEFT;
			ItemCardLabel_tf.multiline = true;
			ItemCardLabel_tf.wordWrap = true;
		}

		override public function PopulateEntry(param1:Object) : *
		{
			super.PopulateEntry(param1);
			this.Background_mc.height = ItemCardLabel_tf.textHeight + 5;
			trace("[HUDPlusPlus] set description entry text...");
		}
	}
}