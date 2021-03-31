using System;
using System.Globalization;

namespace MelonLoader.TinyJSON
{
	public abstract class Variant : IConvertible
	{
		protected static readonly IFormatProvider FormatProvider = new NumberFormatInfo();


		// ReSharper disable once UnusedMember.Global
		public void Make<T>( out T item )
		{
			JSON.MakeInto( this, out item );
		}


		public T Make<T>()
		{
			T item;
			JSON.MakeInto( this, out item );
			return item;
		}


		public void Populate<T>( T item ) where T : class
		{
			JSON.Populate( this, item );
		}


		// ReSharper disable once InconsistentNaming
		// ReSharper disable once UnusedMember.Global
		public string ToJSON()
		{
			return JSON.Dump( this );
		}


		public virtual TypeCode GetTypeCode()
		{
			return TypeCode.Object;
		}


		public virtual object ToType( Type conversionType, IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to " + conversionType.Name );
		}


		public virtual DateTime ToDateTime( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to DateTime" );
		}


		public virtual bool ToBoolean( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to Boolean" );
		}

		public virtual byte ToByte( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to Byte" );
		}


		public virtual char ToChar( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to Char" );
		}


		public virtual decimal ToDecimal( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to Decimal" );
		}


		public virtual double ToDouble( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to Double" );
		}


		public virtual short ToInt16( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to Int16" );
		}


		public virtual int ToInt32( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to Int32" );
		}


		public virtual long ToInt64( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to Int64" );
		}


		public virtual sbyte ToSByte( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to SByte" );
		}


		public virtual float ToSingle( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to Single" );
		}


		public virtual string ToString( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to String" );
		}


		public virtual ushort ToUInt16( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to UInt16" );
		}


		public virtual uint ToUInt32( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to UInt32" );
		}


		public virtual ulong ToUInt64( IFormatProvider provider )
		{
			throw new InvalidCastException( "Cannot convert " + GetType() + " to UInt64" );
		}


		public override string ToString()
		{
			return ToString( FormatProvider );
		}


		// ReSharper disable once UnusedMemberInSuper.Global
		public virtual Variant this[ string key ]
		{
			get
			{
				throw new NotSupportedException();
			}

			// ReSharper disable once UnusedMember.Global
			set
			{
				throw new NotSupportedException();
			}
		}


		// ReSharper disable once UnusedMemberInSuper.Global
		public virtual Variant this[ int index ]
		{
			get
			{
				throw new NotSupportedException();
			}

			// ReSharper disable once UnusedMember.Global
			set
			{
				throw new NotSupportedException();
			}
		}


		public static implicit operator Boolean( Variant variant )
		{
			return variant.ToBoolean( FormatProvider );
		}


		public static implicit operator Single( Variant variant )
		{
			return variant.ToSingle( FormatProvider );
		}


		public static implicit operator Double( Variant variant )
		{
			return variant.ToDouble( FormatProvider );
		}


		public static implicit operator UInt16( Variant variant )
		{
			return variant.ToUInt16( FormatProvider );
		}


		public static implicit operator Int16( Variant variant )
		{
			return variant.ToInt16( FormatProvider );
		}


		public static implicit operator UInt32( Variant variant )
		{
			return variant.ToUInt32( FormatProvider );
		}


		public static implicit operator Int32( Variant variant )
		{
			return variant.ToInt32( FormatProvider );
		}


		public static implicit operator UInt64( Variant variant )
		{
			return variant.ToUInt64( FormatProvider );
		}


		public static implicit operator Int64( Variant variant )
		{
			return variant.ToInt64( FormatProvider );
		}


		public static implicit operator Decimal( Variant variant )
		{
			return variant.ToDecimal( FormatProvider );
		}


		public static implicit operator String( Variant variant )
		{
			return variant.ToString( FormatProvider );
		}


		public static implicit operator Guid( Variant variant )
		{
			return new Guid( variant.ToString( FormatProvider ) );
		}
	}
}
