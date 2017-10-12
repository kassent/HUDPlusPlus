package
{
	import Shared.GlobalFunc;
	import flash.display.MovieClip;
	import flash.utils.getDefinitionByName;

	public class ItemCard_MultiEntry extends ItemCard_StandardEntry
	{
		public static const DMG_WEAP_ID : String = "$dmg";

		public static const DMG_ARMO_ID : String = "$dr";


		public var EntryHolder_mc : MovieClip;

		public var Background_mc : MovieClip;

		private var currY : Number;

		private const ENTRY_SPACING : Number = -5;

		public function ItemCard_MultiEntry()
		{
			super();
			this.currY = 0;
		}

		public function PopulateMultiEntry(param1 : Array, param2 : String) : *
		{
			var _loc4_ : ItemCard_MultiEntry_Value = null;
			if (ItemCardLabel_tf != null)
			{
				GlobalFunc.SetText(ItemCardLabel_tf,param2,false);
			}
			while (this.EntryHolder_mc.numChildren > 0)
			{
				this.EntryHolder_mc.removeChildAt(0);
			}
			this.currY = 0;
			var count : uint = 0;
			var index : uint = 0;
			while (index < param1.length)
			{
				if (param1[index].hasOwnProperty("damageType") && param1[index].value > 0)
				{
					++count;
					var classRef: Class = getDefinitionByName("ItemCard_MultiEntry_Value") as Class;
					_loc4_ = new classRef();
					_loc4_.Icon_mc.gotoAndStop(param2 == DMG_WEAP_ID ? param1[index].damageType + GlobalFunc.NUM_DAMAGE_TYPES : param1[index].damageType);
					_loc4_.PopulateEntry(param1[index]);
					this.EntryHolder_mc.addChild(_loc4_);
					_loc4_.y = this.currY;
					this.currY = this.currY + (_loc4_.height + this.ENTRY_SPACING);
				}
				index++;
			}
			this.Background_mc.height = this.EntryHolder_mc.height + this.ENTRY_SPACING + (count - 1) * 0.5 + 1;
		}
	}
}
