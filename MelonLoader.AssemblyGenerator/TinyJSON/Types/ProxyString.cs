using System;

namespace MelonLoader.TinyJSON
{
	public sealed class ProxyString : Variant
	{
		readonly string value;


		public ProxyString( string value )
		{
			this.value = value;
		}


		public override string ToString( IFormatProvider provider )
		{
			return value;
		}
	}
}
