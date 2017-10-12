/**
 * A global singleton for the CLIK framework that initializes the various CLIK subsystems (PopUpManager, FocusHandler, etc...).
 */

/**************************************************************************

Filename    :   CLIK.as

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/
 
package scaleform.clik.core {
    
    import flash.display.DisplayObjectContainer;
    import flash.display.MovieClip;
    import flash.display.DisplayObject;
    import flash.display.Sprite;
    import flash.utils.Dictionary;
    
    import scaleform.clik.managers.FocusHandler;
    import scaleform.clik.managers.PopUpManager;
    
    import flash.display.Stage;
    import flash.events.Event;
    import flash.events.EventPhase;
    
    import scaleform.gfx.Extensions;
    
    dynamic public class CLIK { // Consider more verbose name: "CLIKState", "CLIKGlobal", etc.
        
    // Constants:
    
    // Public Properties:
        public static var stage:Stage;
        public static var initialized:Boolean = false;
        public static var rendering:Boolean = false;
        
        /** true if CLIK FocusHandler should never set focus to null or Stage. false to follow the default Flash focus behavior. */
        public static var disableNullFocusMoves:Boolean = false;
        /** true if CLIK FocusHandler should never set focus to "dynamic" TextFields. false to follow the default Flash focus behavior. */
        public static var disableDynamicTextFieldFocus:Boolean = false;
        
        protected static var initStack:Dictionary;
        protected static var isInitListenerActive:Boolean = false;      
        protected static var initStackSize:uint = 0;
        
    // Protected Properties:
        
    // Initialization:
        public static function initialize(stage:Stage, component:UIComponent) {
            if (initialized) { return; }
            CLIK.stage = stage;
            Extensions.enabled = true;
            initialized = true;
            
            FocusHandler.init(stage, component);
            PopUpManager.init(stage);
            
            initStack = new Dictionary( true );
        }
        
        public static function queueInitCallback( ref:UIComponent ):void {
            initStack[ initStackSize++ ] = ref;
            
            if (!isInitListenerActive) {
                stage.addEventListener( Event.EXIT_FRAME, fireInitCallback, false, 0, true );
            }
        }
        
        protected static function fireInitCallback( e:Event ):void {
            while ( initStackSize > 0 ) {
                var ref:UIComponent = initStack[ --initStackSize ];
                Extensions.CLIK_addedToStageCallback( ref.name, getTargetPathFor( ref ), ref );
            }
            
            stage.removeEventListener( Event.EXIT_FRAME, fireInitCallback, false );
            isInitListenerActive = false;
        }
        
    // Public Getter / Setters:
    
    // Public Methods:
        public static function getTargetPathFor(clip:DisplayObjectContainer):String {
            if (!clip.parent) {
                return clip.name;
            }
            else {
                var targetPath:String = clip.name;
                return getTargetPathImpl(clip.parent as DisplayObjectContainer, targetPath);
            }
        }
        
        protected static function getTargetPathImpl(clip:DisplayObjectContainer, targetPath:String = ""):String {
            if (!clip) {
                return targetPath; 
            }
            else {
                var _name:String = (clip.name) ? (clip.name + ".") : "";
                targetPath = _name + targetPath;
                return getTargetPathImpl(clip.parent as DisplayObjectContainer, targetPath);
            }
        }
        
    // Protected Methods:
        
    }
    
}