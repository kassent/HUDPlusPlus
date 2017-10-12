package  {
	
	import Shared.GlobalFunc;
	import flash.display.MovieClip;
	import flash.text.TextField;
	import scaleform.gfx.Extensions;
	import scaleform.gfx.TextFieldEx;
	
	public class PlayerStateWidget extends MovieClip {
		
		public static const STRING_INFINITE : String = "$infinite";

		public var ItemWeight_tf : TextField;//ItemCardLabel_tf

		public var ItemValue_tf : TextField;
		
		public var ItemRatio_tf : TextField;
		
		public function PlayerStateWidget() 
		{
			super();
			Extensions.enabled = true;
			if (ItemWeight_tf != null)
			{
				TextFieldEx.setTextAutoSize(ItemWeight_tf, TextFieldEx.TEXTAUTOSZ_SHRINK);
			}
			if (ItemValue_tf != null)
			{
				TextFieldEx.setTextAutoSize(ItemValue_tf, TextFieldEx.TEXTAUTOSZ_SHRINK);
			}
			if (ItemRatio_tf != null)
			{
				TextFieldEx.setTextAutoSize(ItemRatio_tf, TextFieldEx.TEXTAUTOSZ_SHRINK);
			}
		}
		
		public function SetText(param: Object) : void
		{
			//currentWeight totalWeight itemWeight //number
			trace("[HUDPlusPlus] set text enter...")
			var carryweightState :String = "";
			if(param.currentWeight > param.totalWeight)
			{
				carryweightState += "<font color='#ff0000'>" + uint(param.currentWeight) + "</font>";
			}
			else
			{
				carryweightState += uint(param.currentWeight);
			}
			if(param.currentWeight + param.itemWeight > param.totalWeight)
			{
				carryweightState += "<font color='#ff0000'>(+" + uint(param.itemWeight) + ")</font>";
			}
			else
			{
				carryweightState += "(+" + uint(param.itemWeight) + ")";
			}
			carryweightState += "/" + uint(param.totalWeight);
			GlobalFunc.SetText(ItemWeight_tf, carryweightState, true);
			GlobalFunc.SetText(ItemValue_tf, String(param.currentGolds) + "(+" + String(param.itemValue) + ")", false);
			if(param.itemRatio < 0)
			{
				GlobalFunc.SetText(ItemRatio_tf, STRING_INFINITE, false);
			}
			else
			{
				GlobalFunc.SetText(ItemRatio_tf, String(param.itemRatio), false);
			}
			trace("[HUDPlusPlus] set text enter...")
		}
		
		
		public function ToggleElements(show : Boolean) : void
		{
			if(show)
			{
				ItemWeight_tf.visible = true;
				ItemValue_tf.visible = true;
				ItemRatio_tf.visible = true;
			}
			else
			{
				ItemWeight_tf.visible = false;
				ItemValue_tf.visible = false;
				ItemRatio_tf.visible = false;
			}
		}
	}
}
