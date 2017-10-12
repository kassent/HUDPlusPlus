package
{
	import Shared.AS3.BSUIComponent;
	import flash.utils.getDefinitionByName;
	import flash.display.MovieClip;
	import flash.geom.Point;
	import flash.events.Event;
	import scaleform.clik.utils.Padding;

	public class ItemCard extends BSUIComponent
	{
		public var _InfoObj : Array;
		
		public var _isQuickContainerOpen : Boolean = false;
		
		private var _itemCardOffsetX : uint = 0;
		
		private var _baseCoordY : Number = 0;
		
		private var _canDisplay : Boolean = true;
		
		
		private var currY : Number;

		private const ENTRY_SPACING : Number = -3.5;

		private const ET_STANDARD : uint = 0;

		private const ET_AMMO : uint = 1;

		private const ET_DMG_WEAP : uint = 2;

		private const ET_DMG_ARMO : uint = 3;

		private const ET_TIMED_EFFECT : uint = 4;

		private const ET_COMPONENT : uint = 5;

		private const ET_ITEM_DESCRIPTION : uint = 6;
		
		private var QuickContainerWidget_mc : MovieClip;
		
		private var RolloverWidget_mc : MovieClip;

		public function ItemCard()
		{
			super();
			this._InfoObj = new Array();
			this.currY = 0;
			addEventListener(Event.ADDED_TO_STAGE, this.onAddedtoStageHandler);
		}

		private function onAddedtoStageHandler(e: Event): void
		{
			//trace("[HUDPlusPlus] itemcard::enter stage handler...");
			removeEventListener(Event.ADDED_TO_STAGE,this.onAddedtoStageHandler);	
			
			RolloverWidget_mc  = stage.getChildAt(0)["CenterGroup_mc"]["RolloverWidget_mc"];
			QuickContainerWidget_mc = stage.getChildAt(0)["CenterGroup_mc"]["QuickContainerWidget_mc"];
			//this.scaleX = 0.7;
			//this.scaleY = 0.7;
			//trace("[HUDPlusPlus] itemcard::leave stage handler...");
		}
		
		
		public function get InfoObj() : Array
		{
			return this._InfoObj;
		}

		public function set InfoObj(param1 : Array) : *
		{
			this._InfoObj = param1;
		}
		
		public function get itemCardOffsetX() : uint
		{
			return this._itemCardOffsetX;
		}

		public function set itemCardOffsetX(param1 : uint) : *
		{
			this._itemCardOffsetX = param1;
		}
				
		
		public function get isQuickContainerOpen() : Boolean
		{
			return this._isQuickContainerOpen;
		}

		public function set isQuickContainerOpen(param1 : Boolean) : *
		{
			this._isQuickContainerOpen = param1;
		}
		
		//_baseCoordY
		public function get baseCoordY() : Number
		{
			return this._baseCoordY;
		}

		public function set baseCoordY(param1 : Number) : *
		{
			this._baseCoordY = param1;
		}		
		
		//_canDisplay
		public function get canDisplay() : Boolean
		{
			return this._canDisplay;
		}

		public function set canDisplay(param1 : Boolean) : *
		{
			this._canDisplay = param1;
		}		

		
		public function onDataChange() : *
		{
			SetIsDirty();
		}

		override public function redrawUIComponent() : void
		{
			var itemCardEntry : MovieClip = null;
			var itemCardEntryType : uint = 0;
			super.redrawUIComponent();
			while (this.numChildren > 0)
			{
				this.removeChildAt(0);
			}
			this.currY = 0;
			var isWeaponEntry : Boolean = false;
			var isArmorEntry: Boolean = false;
			var showDescription : Boolean = false;
			var index : int = this._InfoObj.length - 1;
			var descObj : Object = null;
			while (index >= 0)
			{
				if(this._InfoObj[index].hasOwnProperty("damageType"))
				{
					if(this._InfoObj[index].hasOwnProperty("isWeapon"))
					{
						isWeaponEntry = true;
					}
					else if(this._InfoObj[index].hasOwnProperty("isArmor"))
					{
						isArmorEntry = true;
					}
				}
				else if(_InfoObj[index].hasOwnProperty("isDescription"))
				{
					descObj = _InfoObj[index];
				}
				else
				{
					itemCardEntryType = this.GetEntryType(this._InfoObj[index]);
					itemCardEntry = this.CreateEntry(itemCardEntryType);
					if (itemCardEntry != null)
					{
						itemCardEntry.PopulateEntry(this._InfoObj[index]);
						addChild(itemCardEntry);
						itemCardEntry.y = this.currY - itemCardEntry.height;
						this.currY = this.currY - (itemCardEntry.height + this.ENTRY_SPACING);
					}
				}
				index--;
			}
			if (isWeaponEntry)
			{
				itemCardEntry = this.CreateEntry(this.ET_DMG_WEAP);
				if (itemCardEntry != null)
				{
					(itemCardEntry as ItemCard_MultiEntry).PopulateMultiEntry(this._InfoObj,ItemCard_MultiEntry.DMG_WEAP_ID);
					addChild(itemCardEntry);
					itemCardEntry.y = this.currY - itemCardEntry.height;
					this.currY = this.currY - (itemCardEntry.height + this.ENTRY_SPACING);
				}
			}
			if (isArmorEntry)
			{
				itemCardEntry = this.CreateEntry(this.ET_DMG_ARMO);
				if (itemCardEntry != null)
				{
					(itemCardEntry as ItemCard_MultiEntry).PopulateMultiEntry(this._InfoObj,ItemCard_MultiEntry.DMG_ARMO_ID);
					addChild(itemCardEntry);
					itemCardEntry.y = this.currY - itemCardEntry.height;
					this.currY = this.currY - (itemCardEntry.height + this.ENTRY_SPACING);
				}
			}
			if(descObj != null)
			{
				//this.currY -= 4;
				itemCardEntry = this.CreateEntry(this.ET_ITEM_DESCRIPTION);
				if (itemCardEntry != null)
				{
					itemCardEntry.PopulateEntry(descObj);
					addChild(itemCardEntry);
					itemCardEntry.y = this.currY - itemCardEntry.height;
					this.currY = this.currY - (itemCardEntry.height + this.ENTRY_SPACING);
				}
			}
			PositionElements();
		}

		private function GetEntryType(param1:Object) : uint
		{
			var entryType : uint = this.ET_STANDARD;
			if(param1.hasOwnProperty("isComponent"))
			{
				entryType = this.ET_COMPONENT;
			}
			else if(param1.hasOwnProperty("isAmmo"))
			{
				entryType = this.ET_AMMO;
			}
			else if(param1.hasOwnProperty("duration"))
			{
				entryType = this.ET_TIMED_EFFECT;
			}
			else if(param1.hasOwnProperty("isDescription"))
			{
				entryType = this.ET_ITEM_DESCRIPTION;	
			}
			return entryType;
		}

		private function CreateEntry(param1:uint) : MovieClip
		{
			var itemCardEntry : MovieClip = null;
			var classRef: Class = null;
			switch (param1)
			{
			case this.ET_STANDARD:
				classRef = getDefinitionByName("ItemCard_StandardEntry") as Class;
				itemCardEntry = new classRef();
				break;
			case this.ET_AMMO:
				classRef = getDefinitionByName("ItemCard_AmmoEntry") as Class;
				itemCardEntry = new classRef();
				break;
			case this.ET_DMG_WEAP:
			case this.ET_DMG_ARMO:
				classRef = getDefinitionByName("ItemCard_MultiEntry") as Class;
				itemCardEntry = new classRef();
				break;
			case this.ET_COMPONENT:
				classRef = getDefinitionByName("ItemCard_ComponentEntry") as Class;
				itemCardEntry = new classRef();
				break;
			case this.ET_TIMED_EFFECT:
				classRef = getDefinitionByName("ItemCard_TimedEntry") as Class;
				itemCardEntry = new classRef();
				break;
			case this.ET_ITEM_DESCRIPTION:
				classRef = getDefinitionByName("ItemCard_DescriptionEntry") as Class;
				itemCardEntry = new classRef();
				break;
			}
			return itemCardEntry;
		}
		
		public function PositionElements() : void
		{
			var coords:Point = new Point(0, 0);
			if(isQuickContainerOpen)
			{
				var BracketPairHolder_mc: MovieClip =  stage.getChildAt(0)["CenterGroup_mc"]["QuickContainerWidget_mc"]["ListHeaderAndBracket_mc"]["BracketPairHolder_mc"];
				var UpperBracketCoords: Point = this.parent.globalToLocal(BracketPairHolder_mc.localToGlobal(new Point(BracketPairHolder_mc.UpperBracket_mc.x + BracketPairHolder_mc.UpperBracket_mc.width + 222, BracketPairHolder_mc.UpperBracket_mc.y)));
				var LowerBracketCoords: Point = this.parent.globalToLocal(BracketPairHolder_mc.localToGlobal(new Point(BracketPairHolder_mc.LowerBracket_mc.x, BracketPairHolder_mc.LowerBracket_mc.y)));
				
				//trace("[HUDPlusPlus] UpperBracketCoords-X: " + UpperBracketCoords.x + "Y:" + UpperBracketCoords.y);
				//trace("[HUDPlusPlus] LowerBracketCoords-X: " + LowerBracketCoords.x + "Y:" + LowerBracketCoords.y);
				//var testPoint = BracketPairHolder_mc.globalToLocal(new Point(stage.stageWidth - 30, 0));
				//trace("[HUDPlusPlus]UpperBracket_mc-x:" + BracketPairHolder_mc.UpperBracket_mc.x + " UpperBracket_mc-width:" + BracketPairHolder_mc.UpperBracket_mc.width + "globalpoint-x:" +  testPoint.x);
				this.y = (LowerBracketCoords.y + UpperBracketCoords.y) / 2 + this.height / 2;
				//this.x = UpperBracketCoords.x + itemCardOffsetX - 200;
			}
			else
			{
				//coords = RolloverWidget_mc.globalToLocal(new Point(stage.stageWidth - 30, stage.stageHeight / 2));
				//this.x = coords.x;
				/////this.x = RolloverWidget_mc["RolloverName_tf"].x + RolloverWidget_mc["RolloverName_tf"].width + itemCardOffsetX - 6;//194;//ButtonHintBar_mc
				//trace("[HUDPlusPlus] itemcard-y=" + coords.y);
				this.y = baseCoordY + this.height / 2;
				//trace("[HUDPlusPlus]RolloverItemCard-x:" + String(this.x) + " RolloverName-x:" + RolloverWidget_mc["RolloverName_tf"].x + " RolloverName-width:" + RolloverWidget_mc["RolloverName_tf"].width);
				/////this.y = RolloverWidget_mc["ButtonHintBar_mc"].y / 2 + RolloverWidget_mc["ButtonHintBar_mc"].height / 4 + this.height / 2 - 5;//coords.y + this.height / 2; RolloverWidget_mc["ButtonHintBar_mc"].y / 2 + RolloverWidget_mc["ButtonHintBar_mc"].height / 4
			}
		}
	}
}