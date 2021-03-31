using System;

namespace MelonLoader.TinyJSON
{
	public sealed class ProxyBoolean : Variant
	{
		readonly bool value;


		public ProxyBoolean( bool value )
		{
			this.value = value;
		}


		public override bool ToBoolean( IFormatProvider provider )
		{
			return value;
		}


		public override string ToString( IFormatProvider provider )
		{
			return value ? "true" : "false";
		}
	}
}
