package  {
	
	import flash.display.MovieClip;
	import flash.events.Event;
	import flash.text.TextField;
	import flash.utils.getDefinitionByName;
	import flash.events.TimerEvent;
	import flash.utils.Timer;
	import flash.display.Sprite;
	import flash.geom.Matrix;
	import flash.geom.Point;
	import flash.geom.ColorTransform;
	import flash.geom.Rectangle;
	import flash.display.DisplayObjectContainer;
	
	public class HUDPlusPlus extends MovieClip 
	{	
		public static const FORM_ARMO : uint = 29;
		
		public static const FORM_BOOK : uint = 30;
		
		public static const FORM_MISC : uint = 35;
		
		public static const FORM_WEAP : uint = 43;
		
		public static const FORM_AMMO : uint = 44;
		
		public static const FORM_KEYM : uint = 47;
		
		public static const FORM_ALCH : uint = 48;
		
		public static const FORM_NOTE : uint = 50;
		
		
		private var HUDMenu: MovieClip;
		
		private var RolloverWidget: MovieClip;
		
		private var CenterGroup_mc : MovieClip;
		
		private var CrosshairBase_mc : MovieClip;
		
		private var QuickContainerWidget_mc : MovieClip;
		
		private var CodeObj: Object;
		
		
		private var widgetContainer : Sprite = new Sprite();

		private var targetSurface : DisplayObjectContainer;
		
		private var crosshairItemCard : MovieClip;
		
		private var quickContainerItemCard : MovieClip;
		
		private var briefItemInfoWidget : MovieClip;
		
		private var itemIconsWidget: MovieClip;
		
		//bEnableGFXLog=1 f4se.ini
		
		//private var crosshairCoordOffsetY : Number;
		
		private var isInit : Boolean = false;

		public function HUDPlusPlus() 
		{
			super();
			addEventListener(Event.ADDED_TO_STAGE, this.onAddedtoStageHandler);
		}
		
		private function onAddedtoStageHandler(e: Event): void
		{
			removeEventListener(Event.ADDED_TO_STAGE,this.onAddedtoStageHandler);
			//Init property...
			HUDMenu = stage.getChildAt(0);	
			CodeObj = HUDMenu["f4se"].plugins.HUDPlusPlus;	
			
			CenterGroup_mc = HUDMenu["CenterGroup_mc"];
			RolloverWidget = CenterGroup_mc["RolloverWidget_mc"];
			CrosshairBase_mc = CenterGroup_mc["HUDCrosshair_mc"]["CrosshairBase_mc"];
			QuickContainerWidget_mc = CenterGroup_mc["QuickContainerWidget_mc"];

			//CrosshairBase_mc.x = crosshairTargetPoint.x;

			targetSurface = HUDMenu.CenterGroup_mc.HUDCrosshair_mc.CrosshairBase_mc;

			CreateTargetSurface(targetSurface.CrosshairTicks_mc.Up);
			CreateTargetSurface(targetSurface.CrosshairClips_mc);
			targetSurface.addChild(widgetContainer);
		
			var classRef = getDefinitionByName("ItemCard") as Class;
			quickContainerItemCard = new classRef();
			quickContainerItemCard.name = "HUDPlusPlus_ItemCard_QuickContainer";
			widgetContainer.addChild(quickContainerItemCard);
			quickContainerItemCard.isQuickContainerOpen = true;	

			classRef = getDefinitionByName("BriefItemInfoWidget") as Class;
			briefItemInfoWidget = new classRef();
			briefItemInfoWidget.name = "HUDPlusPlus_BriefItemInfoWidget";
			briefItemInfoWidget.visible = true;
			RolloverWidget.addChild(briefItemInfoWidget);
			
			classRef = getDefinitionByName("ItemCard") as Class;
			crosshairItemCard = new classRef();
			crosshairItemCard.name = "HUDPlusPlus_ItemCard_Crosshair";
			RolloverWidget.addChild(crosshairItemCard);
			crosshairItemCard.isQuickContainerOpen = false;	

			classRef = getDefinitionByName("ItemIconsWidget") as Class;
			itemIconsWidget = new classRef();
			itemIconsWidget.name = "HUDPlusPlus_itemIconsWidget";
			itemIconsWidget.visible = false;
			RolloverWidget.addChild(itemIconsWidget);
			isInit = true;

			try
			{
				var settings : Object = CodeObj.getModSettings();
				onModSettingChange(settings);
			}
			catch(e : Error)
			{
				
			}
			//trace("[HUDPlusPlus] enter stage handler...");
		}
		
		// Ensures that no matter how the target surface (e.g. compass) is warped or scaled, our widgets appear correctly on the screen.
		// Retrieves the transformation matrix of the target surface, inverts it, then applies it to the widget container.
		public function CreateTargetSurface(targetSurface : MovieClip) : void
		{
			var bgRC = new Sprite();
			bgRC.name = "HUDPlusPlus_Surface";
			bgRC.graphics.beginFill(0x00FF00, 0.0);
			bgRC.graphics.drawRect(0, 0, stage.stageWidth, stage.stageHeight * 1.25);
			bgRC.graphics.endFill();
			
			targetSurface.addChild(bgRC);
			
			var targetSurfaceMatrix:Matrix = targetSurface.transform.concatenatedMatrix.clone();
			// a and d properties of the matrix affects the positioning of pixels along the x and y axis respectively when scaling or rotating an object.
			if (targetSurfaceMatrix.a != 1 || targetSurfaceMatrix.d != 1 || widgetContainer.transform.matrix.a != 1 || widgetContainer.transform.matrix.d != 1) {
				targetSurfaceMatrix.invert();
				bgRC.transform.matrix = targetSurfaceMatrix;
			}
			var globalCoords = targetSurface.globalToLocal(new Point(0, 0));
			bgRC.x = globalCoords.x;
			bgRC.y = globalCoords.y;
		}
		
		public function onModSettingChange(settings : Object) : void
		{
			if(!isInit)
				return;
			if(settings.bShowCrosshair == true)
			{
				targetSurface.CrosshairTicks_mc.alpha = 1.0;
				targetSurface.CrosshairClips_mc.alpha = 1.0;
			}
			else
			{
				targetSurface.CrosshairTicks_mc.alpha = 0.001;
				targetSurface.CrosshairClips_mc.alpha = 0.001;
				//CreateTargetSurface(targetSurface.CrosshairTicks_mc.Up);
				//CreateTargetSurface(targetSurface.CrosshairClips_mc);
			}
			var position : Point = crosshairItemCard.parent.globalToLocal(new Point(settings.iCrosshairItemCardCoordX, settings.iCrosshairItemCardCoordY));
			crosshairItemCard.x = position.x;
			crosshairItemCard.baseCoordY = position.y;
			crosshairItemCard.scaleX = crosshairItemCard.scaleY = settings.fCrosshairItemCardScale;
			crosshairItemCard.canDisplay = settings.bCrosshairItemCardVisible;//bool
			//crosshairItemCard.alpha = settings.fCrosshairItemCardAlpha;
			
			position = quickContainerItemCard.parent.globalToLocal(new Point(settings.iQuickContainerItemCardCoordX, stage.stageHeight / 2));
			quickContainerItemCard.x = position.x;
			quickContainerItemCard.scaleX = quickContainerItemCard.scaleY = settings.fQuickContainerItemCardScale;
			quickContainerItemCard.canDisplay = settings.bQuickContainerItemCardVisible;
			//quickContainerItemCard.alpha = settings.fQuickContainerItemCardAlpha;
						
			position = briefItemInfoWidget.parent.globalToLocal(new Point(settings.iBriefItemInfoWidgetCoordX, settings.iBriefItemInfoWidgetCoordY));
			briefItemInfoWidget.x = position.x;
			briefItemInfoWidget.y = position.y;
			briefItemInfoWidget.scaleX = briefItemInfoWidget.scaleY = settings.fBriefItemInfoWidgetScale;
			briefItemInfoWidget.canDisplay = settings.bBriefItemInfoWidgetVisible;
			
			itemIconsWidget.canDisplay = itemIconsWidget.visible = settings.bSpecialItemIconVisible;
			
			//trace("[HUDPlusPlus] onModSettingChange...");	
		}
		
		public function onCrosshairRefEnter(param1: Array, param2: Object) : void
		{
			if(!isInit)
				return;
			if(crosshairItemCard.canDisplay == true)
			{
				crosshairItemCard.InfoObj = param1;
				//crosshairItemCard.itemCardOffsetX = param2.iRolloverItemCardOffsetX;
				//if(crosshairItemCard.scaleX != param2.iRolloverItemCardScale / 100)
				//{
				//	crosshairItemCard.scaleX = crosshairItemCard.scaleY = param2.iRolloverItemCardScale / 100;
				//}
				crosshairItemCard.onDataChange();
				
				crosshairItemCard.visible = true;				
			}

			if(briefItemInfoWidget.canDisplay && param2.hasOwnProperty("briefItemInfo"))
			{
				briefItemInfoWidget.setText(param2["briefItemInfo"]);
				//var lastCharIndex: int = RolloverWidget["RolloverName_tf"].length - 1;
				//var charRectangle: Rectangle = RolloverWidget["RolloverName_tf"].getCharBoundaries(lastCharIndex);
				//briefItemInfoWidget.y = (RolloverWidget["ButtonHintBar_mc"].y + (RolloverWidget["RolloverName_tf"].y + RolloverWidget["RolloverName_tf"].height) - briefItemInfoWidget.height) / 2 + 1;
				//briefItemInfoWidget.x = RolloverWidget["RolloverName_tf"].x + RolloverWidget["RolloverName_tf"].width / 2 - 100 + param2.iBriefItemCardOffsetX;//iBriefItemCardOffsetX;
				briefItemInfoWidget.visible = true;
				
			}
			else
			{
				briefItemInfoWidget.visible = false;
			}
			
			if(itemIconsWidget.canDisplay && (param2.hasOwnProperty("isRead") || param2.hasOwnProperty("isQuestItem") || param2.hasOwnProperty("isSkillBook")))
			{
				itemIconsWidget.x = 15 + RolloverWidget["RolloverName_tf"].getLineMetrics(0).width + RolloverWidget["RolloverName_tf"].getLineMetrics(0).x + RolloverWidget["RolloverName_tf"].x;
				itemIconsWidget.y = RolloverWidget["RolloverName_tf"].getLineMetrics(0).y + RolloverWidget["RolloverName_tf"].getLineMetrics(0).height / 2;
				itemIconsWidget.PositionElements(param2.hasOwnProperty("isRead"), param2.hasOwnProperty("isQuestItem"), param2.hasOwnProperty("isSkillBook"));
				itemIconsWidget.visible = true;
			}
			else
			{
				itemIconsWidget.visible = false;
			}
			//trace("[HUDPlusPlus] onCrosshairRefEnter...");			
		}
		
		public function onCrosshairRefLeave() : void
		{
			if(!isInit)
				return;
			crosshairItemCard.visible = false;
			itemIconsWidget.visible = false;
			briefItemInfoWidget.visible = false;
			//trace("[HUDPlusPlus] onCrosshairRefLeave...");	
		}		
		
		
		public function onQuickContainerEnter(param1: Array, param2: Object) : void
		{
			if(!isInit)
				return;
			if(quickContainerItemCard.canDisplay)
			{
				quickContainerItemCard.InfoObj = param1;
				//quickContainerItemCard.itemCardOffsetX = param2.iQuickContainerItemCardOffsetX;
				//if(quickContainerItemCard.scaleX != param2.iQuickContainerItemCardScale / 100)
				//{
				//	quickContainerItemCard.scaleX = quickContainerItemCard.scaleY = param2.iQuickContainerItemCardScale / 100;
				//}
				quickContainerItemCard.onDataChange();
				quickContainerItemCard.visible = true;	
			}
			//trace("[HUDPlusPlus] onQuickContainerEnter...");	
		}
		
		
		public function onQuickContainerLeave() : void
		{
			if(!isInit)
				return;
			quickContainerItemCard.visible = false;
			//trace("[HUDPlusPlus] onQuickContainerLeave...");	
		}

		
		public function SetPowerArmorState(isInPA : Boolean) : void
		{
			/*
			//trace("[HUDPlusPlus] set power armor state enter...");
			if(!isInit)	
				return;
			if (!isInPA && this.isInPAState) 
			{
				targetSurface = HUDMenu.BottomCenterGroup_mc.CompassWidget_mc.CompassBar_mc;
				HUDMenu.CenterGroup_mc.HUDCrosshair_mc.CrosshairBase_mc.CrosshairClips_mc.removeChild(powerArmorBGA);
				HUDMenu.CenterGroup_mc.HUDCrosshair_mc.CrosshairBase_mc.CrosshairTicks_mc.Up.removeChild(powerArmorBGB);
				this.isInPAState = false;
			}
			else if(isInPA && !this.isInPAState)
			{
				targetSurface = HUDMenu.CenterGroup_mc.HUDCrosshair_mc.CrosshairBase_mc;

				HUDMenu.CenterGroup_mc.HUDCrosshair_mc.CrosshairBase_mc.CrosshairClips_mc.addChild(powerArmorBGA);
				var coords1 : Point = HUDMenu.CenterGroup_mc.HUDCrosshair_mc.CrosshairBase_mc.CrosshairClips_mc.globalToLocal(new Point(0, 0));
				powerArmorBGA.x = coords1.x;
				powerArmorBGA.y = coords1.y;

				HUDMenu.CenterGroup_mc.HUDCrosshair_mc.CrosshairBase_mc.CrosshairTicks_mc.Up.addChild(powerArmorBGB);
				var coords2 : Point = HUDMenu.CenterGroup_mc.HUDCrosshair_mc.CrosshairBase_mc.CrosshairTicks_mc.Up.globalToLocal(new Point(0, 0));
				powerArmorBGB.x = coords2.x;
				powerArmorBGB.y = coords2.y;
				this.isInPAState = true;
			}
			targetSurface.addChild(widgetContainer);
			this.updateTransformationMatrix();
			*/
		}
	}	
}
