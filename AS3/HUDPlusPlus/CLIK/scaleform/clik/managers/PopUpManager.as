/**************************************************************************

Filename    :   PopUpManager.as

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

﻿package scaleform.clik.managers {
    
    import flash.display.DisplayObject;
    import flash.display.DisplayObjectContainer;
    import flash.display.MovieClip;
    import flash.display.Sprite;
    import flash.display.Stage;
    import flash.events.Event;
    import flash.geom.Point;
    import scaleform.gfx.FocusManager;
    
    public class PopUpManager {
   		protected static var initialized:Boolean = false;
        public static function init(stage:Stage):void {
			if (initialized) { return; }
            PopUpManager._stage = stage;
            _defaultPopupCanvas = new MovieClip();
            _stage.addChild(_defaultPopupCanvas);
            stage.addEventListener(Event.ADDED, PopUpManager.handleStageAddedEvent, false, 0, true);
			initialized = true;
        }
        
    // Constants:
    
    // Protected Properties:
        protected static var _stage:Stage;
        protected static var _defaultPopupCanvas:MovieClip;
        
    // Public Methods:
        public static function show(mc:DisplayObject, x:Number = 0, y:Number = 0, scope:DisplayObjectContainer = null):void {
            if (!_stage) { trace("PopUpManager has not been initialized. Automatic initialization has not occured or has failed; call PopUpManager.init() manually."); return; }
            
            // Remove from existing parent
            if (mc.parent) { mc.parent.removeChild(mc); }
            
            // Reparent to popup canvas layer
            handleStageAddedEvent(null);
            _defaultPopupCanvas.addChild(mc);
            
            // Move to location by scope
            if (!scope) scope = _stage;
            var p:Point = new Point(x, y);
            p = scope.localToGlobal(p);
            mc.x = p.x;
            mc.y = p.y;
        }
        
        public static function showModal(mc:Sprite, bg:Sprite = null, controllerIdx:uint = 0):void {
            if (!_stage) { trace("PopUpManager has not been initialized. Automatic initialization has not occured or has failed; call PopUpManager.init() manually."); return; }
            
            // Remove from existing parents
            if (mc.parent) { mc.parent.removeChild(mc); }
            if (bg && bg.parent) { bg.parent.removeChild(mc); }
            
            // Reparent to popup canvas layer
            if (bg) _defaultPopupCanvas.addChild(bg);
            _defaultPopupCanvas.addChild(mc);
            
            FocusManager.setModalClip(mc, controllerIdx);
        }
        
    // Protected Methods:
        protected static function handleStageAddedEvent(e:Event):void {
            // We make sure that the defaultPopupCanvas is always on top of everything else on the stage
            _stage.setChildIndex(_defaultPopupCanvas, _stage.numChildren - 1);
        }
    }
    
}