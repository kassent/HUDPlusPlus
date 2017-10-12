package scaleform.gfx 
{
    import flash.text.TextField;
	import flash.display.*;
    
    public class TextFieldEx extends InteractiveObjectEx
    {
        public function TextFieldEx()
        {
            super();
        }

        public static function getNoTranslate(arg1:flash.text.TextField):Boolean { return false; }
        public static function setBidirectionalTextEnabled(arg1:flash.text.TextField, arg2:Boolean):void { }
        public static function getBidirectionalTextEnabled(arg1:flash.text.TextField):Boolean { return false; }
        public static function setSelectionTextColor(arg1:flash.text.TextField, arg2:uint):void { }
        public static function getSelectionTextColor(arg1:flash.text.TextField):uint { return 0xFFFFFFFF; }
        public static function setSelectionBkgColor(arg1:flash.text.TextField, arg2:uint):void { }
        public static function getSelectionBkgColor(arg1:flash.text.TextField):uint { return 0xFF000000; }
        public static function setInactiveSelectionTextColor(arg1:flash.text.TextField, arg2:uint):void { }
        public static function getInactiveSelectionTextColor(arg1:flash.text.TextField):uint { return 0xFFFFFFFF; }
        public static function setInactiveSelectionBkgColor(arg1:flash.text.TextField, arg2:uint):void { }
        public static function getInactiveSelectionBkgColor(arg1:flash.text.TextField):uint { return 0xFF000000; }
        public static function appendHtml(arg1:flash.text.TextField, arg2:String):void { }
        public static function setIMEEnabled(arg1:flash.text.TextField, arg2:Boolean):void { }
        public static function setVerticalAlign(arg1:flash.text.TextField, arg2:String):void { }
        public static function getVerticalAlign(arg1:flash.text.TextField):String { return "none"; }
        public static function setVerticalAutoSize(arg1:flash.text.TextField, arg2:String):void { }
        public static function getVerticalAutoSize(arg1:flash.text.TextField):String { return "none"; }
        public static function setTextAutoSize(arg1:flash.text.TextField, arg2:String):void{ }
        public static function getTextAutoSize(arg1:flash.text.TextField):String { return "none"; }
        public static function setImageSubstitutions(arg1:flash.text.TextField, arg2:Object):void { }
        public static function updateImageSubstitution(arg1:flash.text.TextField, arg2:String, arg3:flash.display.BitmapData):void { }
        public static function setNoTranslate(arg1:flash.text.TextField, arg2:Boolean):void { }

        public static const VALIGN_NONE:String="none";
        public static const VALIGN_TOP:String="top";
        public static const VALIGN_CENTER:String="center";
        public static const VALIGN_BOTTOM:String="bottom";
        public static const TEXTAUTOSZ_NONE:String="none";
        public static const TEXTAUTOSZ_SHRINK:String="shrink";
        public static const TEXTAUTOSZ_FIT:String="fit";
        public static const VAUTOSIZE_NONE:String="none";
        public static const VAUTOSIZE_TOP:String="top";
        public static const VAUTOSIZE_CENTER:String="center";
        public static const VAUTOSIZE_BOTTOM:String="bottom";
    }
}
