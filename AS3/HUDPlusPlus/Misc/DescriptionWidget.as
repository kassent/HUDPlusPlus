package
{
    import Shared.*;
    import flash.display.*;
    import flash.text.*;
    import scaleform.gfx.*;

	public class DescriptionWidget extends flash.display.MovieClip
	{
		public var textField:flash.text.TextField;

        protected var _clipIndex:uint;

        protected var _itemIndex:uint;

        protected var _selected:Boolean;
		
		public function DescriptionWidget()
		{
            super();
            scaleform.gfx.Extensions.enabled = true;
			Shared.GlobalFunc.SetText(this.textField, "   ", true);
		}


		public function SetText(param: String) : void
		{
			scaleform.gfx.TextFieldEx.setTextAutoSize(this.textField, "shrink");
			Shared.GlobalFunc.SetText(this.textField, param, true);
		}
	}
}
