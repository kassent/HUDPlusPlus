package
{
	import Shared.GlobalFunc;
	import flash.display.MovieClip;
	import flash.text.TextField;
	import scaleform.gfx.Extensions;
	import scaleform.gfx.TextFieldEx;
	
	public class ItemCard_TimedEntry extends ItemCard_StandardEntry
	{
//$for
      	public static const DURATION_SEPERATOR: String = "$for";

		//public var TimerIcon_mc : MovieClip;
		public var ItemCardMag_tf : TextField;//ItemCardLabel_tf

		public var ItemCardDur_tf : TextField;
		
		public var Background_mc : MovieClip;
		
		public var IconMag_mc : MovieClip;
		
		public var IconDur_mc : MovieClip;

		public function ItemCard_TimedEntry()
		{
			super();
			if (this.ItemCardMag_tf != null)
			{
				TextFieldEx.setTextAutoSize(this.ItemCardMag_tf,TextFieldEx.TEXTAUTOSZ_SHRINK);
			}
			if (this.ItemCardDur_tf != null)
			{
				TextFieldEx.setTextAutoSize(this.ItemCardDur_tf,TextFieldEx.TEXTAUTOSZ_SHRINK);
			}
		}

		override public function PopulateEntry(param1:Object) : *
		{
			//TimerIcon_mc.visible = false;
			GlobalFunc.SetText(this.ItemCardLabel_tf, param1.key, false);

			if(param1.duration != undefined)
			{
				if(param1.duration != uint(0))
				{
					IconMag_mc.visible = true;
					IconDur_mc.visible = true;
					ItemCardValue_tf.visible = false;
					GlobalFunc.SetText(this.ItemCardMag_tf, param1.magnitude, false);
					var durStr: String = "";
					var duration: uint = param1.duration;
					if(duration < 600)
					{
						durStr += String(duration);
						durStr += "s";
					}
					else if(duration < 3600)
					{
						durStr += String(uint(duration/60));
						durStr += "m";
					}
					else if(duration < 3600 * 24)
					{
						durStr += String(uint(duration/3600));
						durStr += "h";
					}
					else
					{
						durStr += String(uint(duration/(3600 * 24)));
						durStr += "d";
					}
					GlobalFunc.SetText(this.ItemCardDur_tf, durStr, false);
				}
				else
				{
					IconMag_mc.visible = false;
					IconDur_mc.visible = false;
					ItemCardMag_tf.visible = false;
					ItemCardDur_tf.visible = false;
					ItemCardValue_tf.visible = true;
					GlobalFunc.SetText(this.ItemCardValue_tf, param1.magnitude, false);
				}
			}
		}
	}
}