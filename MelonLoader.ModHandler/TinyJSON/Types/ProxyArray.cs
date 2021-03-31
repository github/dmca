using System.Collections;
using System.Collections.Generic;

namespace MelonLoader.TinyJSON
{
	public sealed class ProxyArray : Variant, IEnumerable<Variant>
	{
		readonly List<Variant> list;


		public ProxyArray()
		{
			list = new List<Variant>();
		}


		IEnumerator<Variant> IEnumerable<Variant>.GetEnumerator()
		{
			return list.GetEnumerator();
		}


		IEnumerator IEnumerable.GetEnumerator()
		{
			return list.GetEnumerator();
		}


		public void Add( Variant item )
		{
			list.Add( item );
		}


		public override Variant this[ int index ]
		{
			get
			{
				return list[index];
			}
			set
			{
				list[index] = value;
			}
		}


		public int Count
		{
			get
			{
				return list.Count;
			}
		}


		internal bool CanBeMultiRankArray( int[] rankLengths )
		{
			return CanBeMultiRankArray( 0, rankLengths );
		}


		bool CanBeMultiRankArray( int rank, int[] rankLengths )
		{
			var count = list.Count;
			rankLengths[rank] = count;

			if (rank == rankLengths.Length - 1)
			{
				return true;
			}

			var firstItem = list[0] as ProxyArray;
			if (firstItem == null)
			{
				return false;
			}

			var firstItemCount = firstItem.Count;

			for (var i = 1; i < count; i++)
			{
				var item = list[i] as ProxyArray;

				if (item == null)
				{
					return false;
				}

				if (item.Count != firstItemCount)
				{
					return false;
				}

				if (!item.CanBeMultiRankArray( rank + 1, rankLengths ))
				{
					return false;
				}
			}

			return true;
		}
	}
}
