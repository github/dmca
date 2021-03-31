using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.Reflection;
using System.Text;

namespace MelonLoader.TinyJSON
{
	public sealed class Encoder
	{
		static readonly Type includeAttrType = typeof(Include);
		static readonly Type excludeAttrType = typeof(Exclude);
		static readonly Type typeHintAttrType = typeof(TypeHint);

		readonly StringBuilder builder;
		readonly EncodeOptions options;
		int indent;


		Encoder( EncodeOptions options )
		{
			this.options = options;
			builder = new StringBuilder();
			indent = 0;
		}


		// ReSharper disable once UnusedMember.Global
		public static string Encode( object obj )
		{
			return Encode( obj, EncodeOptions.None );
		}


		public static string Encode( object obj, EncodeOptions options )
		{
			var instance = new Encoder( options );
			instance.EncodeValue( obj, false );
			return instance.builder.ToString();
		}


		bool PrettyPrintEnabled
		{
			get
			{
				return (options & EncodeOptions.PrettyPrint) == EncodeOptions.PrettyPrint;
			}
		}


		bool TypeHintsEnabled
		{
			get
			{
				return (options & EncodeOptions.NoTypeHints) != EncodeOptions.NoTypeHints;
			}
		}


		bool IncludePublicPropertiesEnabled
		{
			get
			{
				return (options & EncodeOptions.IncludePublicProperties) == EncodeOptions.IncludePublicProperties;
			}
		}


		bool EnforceHierarchyOrderEnabled
		{
			get
			{
				return (options & EncodeOptions.EnforceHierarchyOrder) == EncodeOptions.EnforceHierarchyOrder;
			}
		}


		void EncodeValue( object value, bool forceTypeHint )
		{
			if (value == null)
			{
				builder.Append( "null" );
				return;
			}

			if (value is string)
			{
				EncodeString( (string) value );
				return;
			}

			if (value is ProxyString)
			{
				EncodeString( ((ProxyString) value).ToString( CultureInfo.InvariantCulture ) );
				return;
			}

			if (value is char)
			{
				EncodeString( value.ToString() );
				return;
			}

			if (value is bool)
			{
				builder.Append( (bool) value ? "true" : "false" );
				return;
			}

			if (value is Enum)
			{
				EncodeString( value.ToString() );
				return;
			}

			if (value is Array)
			{
				EncodeArray( (Array) value, forceTypeHint );
				return;
			}

			if (value is IList)
			{
				EncodeList( (IList) value, forceTypeHint );
				return;
			}

			if (value is IDictionary)
			{
				EncodeDictionary( (IDictionary) value, forceTypeHint );
				return;
			}

			if (value is Guid)
			{
				EncodeString( value.ToString() );
				return;
			}

			if (value is ProxyArray)
			{
				EncodeProxyArray( (ProxyArray) value );
				return;
			}

			if (value is ProxyObject)
			{
				EncodeProxyObject( (ProxyObject) value );
				return;
			}

			if (value is float ||
			    value is double ||
			    value is int ||
			    value is uint ||
			    value is long ||
			    value is sbyte ||
			    value is byte ||
			    value is short ||
			    value is ushort ||
			    value is ulong ||
			    value is decimal ||
			    value is ProxyBoolean ||
			    value is ProxyNumber)
			{
				builder.Append( Convert.ToString( value, CultureInfo.InvariantCulture ) );
				return;
			}

			EncodeObject( value, forceTypeHint );
		}


		IEnumerable<FieldInfo> GetFieldsForType( Type type )
		{
			if (EnforceHierarchyOrderEnabled)
			{
				var types = new Stack<Type>();
				while (type != null)
				{
					types.Push( type );
					type = type.BaseType;
				}

				var fields = new List<FieldInfo>();
				while (types.Count > 0)
				{
					fields.AddRange( types.Pop().GetFields( BindingFlags.DeclaredOnly | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance ) );
				}

				return fields;
			}

			return type.GetFields( BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance );
		}


		IEnumerable<PropertyInfo> GetPropertiesForType( Type type )
		{
			if (EnforceHierarchyOrderEnabled)
			{
				var types = new Stack<Type>();
				while (type != null)
				{
					types.Push( type );
					type = type.BaseType;
				}

				var properties = new List<PropertyInfo>();
				while (types.Count > 0)
				{
					properties.AddRange( types.Pop().GetProperties( BindingFlags.DeclaredOnly | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance ) );
				}

				return properties;
			}

			return type.GetProperties( BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance );
		}


		void EncodeObject( object value, bool forceTypeHint )
		{
			var type = value.GetType();

			AppendOpenBrace();

			forceTypeHint = forceTypeHint || TypeHintsEnabled;

			var includePublicProperties = IncludePublicPropertiesEnabled;

			var firstItem = !forceTypeHint;
			if (forceTypeHint)
			{
				if (PrettyPrintEnabled)
				{
					AppendIndent();
				}

				EncodeString( ProxyObject.TypeHintKey );
				AppendColon();
				EncodeString( type.FullName );

				// ReSharper disable once RedundantAssignment
				firstItem = false;
			}

			foreach (var field in GetFieldsForType( type ))
			{
				var shouldTypeHint = false;
				var shouldEncode = field.IsPublic;
				foreach (var attribute in field.GetCustomAttributes( true ))
				{
					if (excludeAttrType.IsInstanceOfType( attribute ))
					{
						shouldEncode = false;
					}

					if (includeAttrType.IsInstanceOfType( attribute ))
					{
						shouldEncode = true;
					}

					if (typeHintAttrType.IsInstanceOfType( attribute ))
					{
						shouldTypeHint = true;
					}
				}

				if (shouldEncode)
				{
					AppendComma( firstItem );
					EncodeString( field.Name );
					AppendColon();
					EncodeValue( field.GetValue( value ), shouldTypeHint );
					firstItem = false;
				}
			}

			foreach (var property in GetPropertiesForType( type ))
			{
				if (property.CanRead)
				{
					var shouldTypeHint = false;
					var shouldEncode = includePublicProperties;

					foreach (var attribute in property.GetCustomAttributes( true ))
					{
						if (excludeAttrType.IsInstanceOfType( attribute ))
						{
							shouldEncode = false;
						}

						if (includeAttrType.IsInstanceOfType( attribute ))
						{
							shouldEncode = true;
						}

						if (typeHintAttrType.IsInstanceOfType( attribute ))
						{
							shouldTypeHint = true;
						}
					}

					if (shouldEncode)
					{
						AppendComma( firstItem );
						EncodeString( property.Name );
						AppendColon();
						EncodeValue( property.GetValue( value, null ), shouldTypeHint );
						firstItem = false;
					}
				}
			}

			AppendCloseBrace();
		}


		void EncodeProxyArray( ProxyArray value )
		{
			if (value.Count == 0)
			{
				builder.Append( "[]" );
			}
			else
			{
				AppendOpenBracket();

				var firstItem = true;
				foreach (var obj in value)
				{
					AppendComma( firstItem );
					EncodeValue( obj, false );
					firstItem = false;
				}

				AppendCloseBracket();
			}
		}


		void EncodeProxyObject( ProxyObject value )
		{
			if (value.Count == 0)
			{
				builder.Append( "{}" );
			}
			else
			{
				AppendOpenBrace();

				var firstItem = true;
				foreach (var e in value.Keys)
				{
					AppendComma( firstItem );
					EncodeString( e );
					AppendColon();
					EncodeValue( value[e], false );
					firstItem = false;
				}

				AppendCloseBrace();
			}
		}


		void EncodeDictionary( IDictionary value, bool forceTypeHint )
		{
			if (value.Count == 0)
			{
				builder.Append( "{}" );
			}
			else
			{
				AppendOpenBrace();

				var firstItem = true;
				foreach (var e in value.Keys)
				{
					AppendComma( firstItem );
					EncodeString( e.ToString() );
					AppendColon();
					EncodeValue( value[e], forceTypeHint );
					firstItem = false;
				}

				AppendCloseBrace();
			}
		}


		// ReSharper disable once SuggestBaseTypeForParameter
		void EncodeList( IList value, bool forceTypeHint )
		{
			if (value.Count == 0)
			{
				builder.Append( "[]" );
			}
			else
			{
				AppendOpenBracket();

				var firstItem = true;
				foreach (var obj in value)
				{
					AppendComma( firstItem );
					EncodeValue( obj, forceTypeHint );
					firstItem = false;
				}

				AppendCloseBracket();
			}
		}


		void EncodeArray( Array value, bool forceTypeHint )
		{
			if (value.Rank == 1)
			{
				EncodeList( value, forceTypeHint );
			}
			else
			{
				var indices = new int[value.Rank];
				EncodeArrayRank( value, 0, indices, forceTypeHint );
			}
		}


		void EncodeArrayRank( Array value, int rank, int[] indices, bool forceTypeHint )
		{
			AppendOpenBracket();

			var min = value.GetLowerBound( rank );
			var max = value.GetUpperBound( rank );

			if (rank == value.Rank - 1)
			{
				for (var i = min; i <= max; i++)
				{
					indices[rank] = i;
					AppendComma( i == min );
					EncodeValue( value.GetValue( indices ), forceTypeHint );
				}
			}
			else
			{
				for (var i = min; i <= max; i++)
				{
					indices[rank] = i;
					AppendComma( i == min );
					EncodeArrayRank( value, rank + 1, indices, forceTypeHint );
				}
			}

			AppendCloseBracket();
		}


		void EncodeString( string value )
		{
			builder.Append( '\"' );

			var charArray = value.ToCharArray();
			foreach (var c in charArray)
			{
				switch (c)
				{
					case '"':
						builder.Append( "\\\"" );
						break;

					case '\\':
						builder.Append( "\\\\" );
						break;

					case '\b':
						builder.Append( "\\b" );
						break;

					case '\f':
						builder.Append( "\\f" );
						break;

					case '\n':
						builder.Append( "\\n" );
						break;

					case '\r':
						builder.Append( "\\r" );
						break;

					case '\t':
						builder.Append( "\\t" );
						break;

					default:
						var codepoint = Convert.ToInt32( c );
						if ((codepoint >= 32) && (codepoint <= 126))
						{
							builder.Append( c );
						}
						else
						{
							builder.Append( "\\u" + Convert.ToString( codepoint, 16 ).PadLeft( 4, '0' ) );
						}

						break;
				}
			}

			builder.Append( '\"' );
		}


		#region Helpers

		void AppendIndent()
		{
			for (var i = 0; i < indent; i++)
			{
				builder.Append( '\t' );
			}
		}


		void AppendOpenBrace()
		{
			builder.Append( '{' );

			if (PrettyPrintEnabled)
			{
				builder.Append( '\n' );
				indent++;
			}
		}


		void AppendCloseBrace()
		{
			if (PrettyPrintEnabled)
			{
				builder.Append( '\n' );
				indent--;
				AppendIndent();
			}

			builder.Append( '}' );
		}


		void AppendOpenBracket()
		{
			builder.Append( '[' );

			if (PrettyPrintEnabled)
			{
				builder.Append( '\n' );
				indent++;
			}
		}


		void AppendCloseBracket()
		{
			if (PrettyPrintEnabled)
			{
				builder.Append( '\n' );
				indent--;
				AppendIndent();
			}

			builder.Append( ']' );
		}


		void AppendComma( bool firstItem )
		{
			if (!firstItem)
			{
				builder.Append( ',' );

				if (PrettyPrintEnabled)
				{
					builder.Append( '\n' );
				}
			}

			if (PrettyPrintEnabled)
			{
				AppendIndent();
			}
		}


		void AppendColon()
		{
			builder.Append( ':' );

			if (PrettyPrintEnabled)
			{
				builder.Append( ' ' );
			}
		}

		#endregion
	}
}
