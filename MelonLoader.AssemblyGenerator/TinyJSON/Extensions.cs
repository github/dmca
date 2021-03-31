using System;
using System.Collections.Generic;

namespace MelonLoader.TinyJSON
{
	public static class Extensions
	{
		public static bool AnyOfType<TSource>( this IEnumerable<TSource> source, Type expectedType )
		{
			if (source == null)
			{
				throw new ArgumentNullException( "source" );
			}

			if (expectedType == null)
			{
				throw new ArgumentNullException( "expectedType" );
			}

			foreach (var item in source)
			{
				if (expectedType.IsInstanceOfType( item ))
				{
					return true;
				}
			}

			return false;
		}
	}
}
