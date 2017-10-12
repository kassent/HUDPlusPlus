package  {
	
	
	import Shared.GlobalFunc;
	import flash.display.MovieClip;
	import flash.text.TextField;
	import scaleform.gfx.Extensions;
	import scaleform.gfx.TextFieldEx;
	import flash.geom.Rectangle;		
	
	public class BriefItemInfoWidget extends MovieClip {
		
		public static const STRING_WEIGHT : String = "$WEIGHT";
		
		public static const STRING_VALUE : String = "$VALUE";
		
		//stage elements
		public var ItemWeightMapKey_tf : TextField;

		public var ItemWeightMapValue_tf : TextField;
		
		public var ItemValueMapKey_tf : TextField;
		
		public var ItemValueMapValue_tf : TextField;
		
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
		
		public function BriefItemInfoWidget() 
		{
			super();
			Extensions.enabled = true;
			if (ItemWeightMapKey_tf != null)
			{
				TextFieldEx.setTextAutoSize(ItemWeightMapKey_tf, TextFieldEx.TEXTAUTOSZ_SHRINK);
				GlobalFunc.SetText(ItemWeightMapKey_tf, STRING_WEIGHT, false);
			}
			if ( ItemWeightMapValue_tf != null)
			{
				TextFieldEx.setTextAutoSize( ItemWeightMapValue_tf, TextFieldEx.TEXTAUTOSZ_SHRINK);
			}
			if (ItemValueMapKey_tf != null)
			{
				TextFieldEx.setTextAutoSize(ItemValueMapKey_tf, TextFieldEx.TEXTAUTOSZ_SHRINK);
				GlobalFunc.SetText(ItemValueMapKey_tf, STRING_VALUE, false);
			}
			if (ItemValueMapValue_tf != null)
			{
				TextFieldEx.setTextAutoSize(ItemValueMapValue_tf, TextFieldEx.TEXTAUTOSZ_SHRINK);
			}
		}
		
		
		public function setText(param : Object) : void
		{
			GlobalFunc.SetText(ItemWeightMapValue_tf, String(param.itemWeight), false);
			GlobalFunc.SetText(ItemValueMapValue_tf, String(param.itemValue), false);	
/*
			ItemWeightMapValue_tf.x = 0;
			var lastCharIndex: int = ItemWeightMapValue_tf.length - 1;
			var charRectangle: Rectangle = ItemWeightMapValue_tf.getCharBoundaries(lastCharIndex);
			ItemWeightMapValue_tf.x = -charRectangle.x - 3;
			ItemWeightMapKey_tf.x = 0;
			lastCharIndex = ItemWeightMapKey_tf.length - 1;
			charRectangle = ItemWeightMapKey_tf.getCharBoundaries(lastCharIndex);
			ItemWeightMapKey_tf.x = ItemWeightMapValue_tf.x -charRectangle.x - 8;
			
			ItemValueMapKey_tf.x = 0;
			lastCharIndex = ItemValueMapKey_tf.length - 1;
			charRectangle = ItemValueMapKey_tf.getCharBoundaries(lastCharIndex);
			
			ItemValueMapKey_tf.x = 5;
			ItemValueMapValue_tf.x = ItemValueMapKey_tf.x + charRectangle.x + 8;
			*/
			
		}
		
	}
	
}
