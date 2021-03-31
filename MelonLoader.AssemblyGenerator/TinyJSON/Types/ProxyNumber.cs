using System;
using System.Globalization;

namespace MelonLoader.TinyJSON
{
	public sealed class ProxyNumber : Variant
	{
		static readonly char[] floatingPointCharacters = { '.', 'e' };
		readonly IConvertible value;


		public ProxyNumber( IConvertible value )
		{
			var stringValue = value as string;
			this.value = stringValue != null ? Parse( stringValue ) : value;
		}


		static IConvertible Parse( string value )
		{
			if (value.IndexOfAny( floatingPointCharacters ) == -1)
			{
				if (value[0] == '-')
				{
					Int64 parsedValue;
					if (Int64.TryParse( value, NumberStyles.Float, NumberFormatInfo.InvariantInfo, out parsedValue ))
					{
						return parsedValue;
					}
				}
				else
				{
					UInt64 parsedValue;
					if (UInt64.TryParse( value, NumberStyles.Float, NumberFormatInfo.InvariantInfo, out parsedValue ))
					{
						return parsedValue;
					}
				}
			}

			Decimal decimalValue;
			if (Decimal.TryParse( value, NumberStyles.Float, NumberFormatInfo.InvariantInfo, out decimalValue ))
			{
				// Check for decimal underflow.
				if (decimalValue == Decimal.Zero)
				{
					Double parsedValue;
					if (Double.TryParse( value, NumberStyles.Float, NumberFormatInfo.InvariantInfo, out parsedValue ))
					{
						if (Math.Abs( parsedValue ) > Double.Epsilon)
						{
							return parsedValue;
						}
					}
				}

				return decimalValue;
			}

			Double doubleValue;
			if (Double.TryParse( value, NumberStyles.Float, NumberFormatInfo.InvariantInfo, out doubleValue ))
			{
				return doubleValue;
			}

			return 0;
		}


		public override bool ToBoolean( IFormatProvider provider )
		{
			return value.ToBoolean( provider );
		}


		public override byte ToByte( IFormatProvider provider )
		{
			return value.ToByte( provider );
		}


		public override char ToChar( IFormatProvider provider )
		{
			return value.ToChar( provider );
		}


		public override decimal ToDecimal( IFormatProvider provider )
		{
			return value.ToDecimal( provider );
		}


		public override double ToDouble( IFormatProvider provider )
		{
			return value.ToDouble( provider );
		}


		public override short ToInt16( IFormatProvider provider )
		{
			return value.ToInt16( provider );
		}


		public override int ToInt32( IFormatProvider provider )
		{
			return value.ToInt32( provider );
		}


		public override long ToInt64( IFormatProvider provider )
		{
			return value.ToInt64( provider );
		}


		public override sbyte ToSByte( IFormatProvider provider )
		{
			return value.ToSByte( provider );
		}


		public override float ToSingle( IFormatProvider provider )
		{
			return value.ToSingle( provider );
		}


		public override string ToString( IFormatProvider provider )
		{
			return value.ToString( provider );
		}


		public override ushort ToUInt16( IFormatProvider provider )
		{
			return value.ToUInt16( provider );
		}


		public override uint ToUInt32( IFormatProvider provider )
		{
			return value.ToUInt32( provider );
		}


		public override ulong ToUInt64( IFormatProvider provider )
		{
			return value.ToUInt64( provider );
		}
	}
}
