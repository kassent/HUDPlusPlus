package
{
	import Shared.GlobalFunc;
	import flash.display.MovieClip;
	import flash.text.TextField;
	import scaleform.gfx.Extensions;
	import scaleform.gfx.TextFieldEx;

	public class ItemCard_StandardEntry extends MovieClip
	{
		public var ItemCardLabel_tf : TextField;//ItemCardLabel_tf

		public var ItemCardValue_tf : TextField;

		public var Comparison_mc : MovieClip;

		public function ItemCard_StandardEntry()
		{
			super();
			Extensions.enabled = true;
			if (this.ItemCardLabel_tf != null)
			{
				TextFieldEx.setTextAutoSize(this.ItemCardLabel_tf,TextFieldEx.TEXTAUTOSZ_SHRINK);
			}
			if (this.ItemCardValue_tf != null)
			{
				TextFieldEx.setTextAutoSize(this.ItemCardValue_tf,TextFieldEx.TEXTAUTOSZ_SHRINK);
			}
		}


		public function PopulateEntry(param1:Object) : *
		{
			var _loc2_ : *= null;
			var _loc3_ : Number = NaN;
			var _loc4_ : uint = 0;
			var _loc5_ : *= undefined;
			if (this.ItemCardLabel_tf != null)
			{
				GlobalFunc.SetText(this.ItemCardLabel_tf, param1.key, false);
			}
			if (this.ItemCardValue_tf != null)
			{
				if (param1.value is String)
				{
					_loc2_ = param1.value;
				}
				else
				{
					_loc3_ = param1.value;
					if (param1.scaleWithDuration)
					{
						_loc3_ = _loc3_ * param1.duration;
					}
					_loc2_ = _loc3_.toString();
					_loc4_ = param1.precision != undefined ? uint(param1.precision) : uint(0);
					_loc5_ = _loc2_.indexOf(".");
					if (_loc5_ > -1)
					{
						if (_loc4_)
						{
							_loc2_ = _loc2_.substring(0,Math.min(_loc5_ + _loc4_ + 1,_loc2_.length));
						}
						else
						{
							_loc2_ = _loc2_.substring(0,_loc5_);
						}
					}
					if (param1.showAsPercent)
					{
						_loc2_ = _loc2_ + "%";
					}
				}
				GlobalFunc.SetText(this.ItemCardValue_tf,_loc2_,false);
			}
		}
	}
}