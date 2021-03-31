using System;

namespace MelonLoader.LightJson.Serialization
{
	/// <summary>
	/// Represents a position within a plain text resource.
	/// </summary>
	public struct TextPosition
	{
		/// <summary>
		/// The column position, 0-based.
		/// </summary>
		public long column;

		/// <summary>
		/// The line position, 0-based.
		/// </summary>
		public long line;
	}
}
