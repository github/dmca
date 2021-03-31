using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.Globalization;

namespace MelonLoader.LightJson.Serialization
{
	using ErrorType = JsonSerializationException.ErrorType;

	/// <summary>
	/// Represents a writer that can write string representations of JsonValues.
	/// </summary>
	public sealed class JsonWriter : IDisposable
	{
		private int indent;
		private bool isNewLine;
		private TextWriter writer;

		/// <summary>
		/// A set of containing all the collection objects (JsonObject/JsonArray) being rendered.
		/// It is used to prevent circular references; since collections that contain themselves
		/// will never finish rendering.
		/// </summary>
		private HashSet<IEnumerable<JsonValue>> renderingCollections;

		/// <summary>
		/// Gets or sets the string representing a indent in the output.
		/// </summary>
		public string IndentString { get; set; }

		/// <summary>
		/// Gets or sets the string representing a space in the output.
		/// </summary>
		public string SpacingString { get; set; }

		/// <summary>
		/// Gets or sets the string representing a new line on the output.
		/// </summary>
		public string NewLineString { get; set; }

		/// <summary>
		/// Gets or sets a value indicating whether JsonObject properties should be written in a deterministic order.
		/// </summary>
		public bool SortObjects { get; set; }

		/// <summary>
		/// Initializes a new instance of JsonWriter.
		/// </summary>
		public JsonWriter() : this(false) { }

		/// <summary>
		/// Initializes a new instance of JsonWriter.
		/// </summary>
		/// <param name="pretty">
		/// A value indicating whether the output of the writer should be human-readable.
		/// </param>
		public JsonWriter(bool pretty)
		{
			if (pretty)
			{
				this.IndentString = "\t";
				this.SpacingString = " ";
				this.NewLineString = "\n";
			}
		}

		private void Initialize()
		{
			this.indent = 0;
			this.isNewLine = true;
			this.writer = new StringWriter();
			this.renderingCollections = new HashSet<IEnumerable<JsonValue>>();
		}

		private void Write(string text)
		{
			if (this.isNewLine)
			{
				this.isNewLine = false;
				WriteIndentation();
			}

			writer.Write(text);
		}

		private void WriteEncodedJsonValue(JsonValue value)
		{
			switch (value.Type)
			{
				case JsonValueType.Null:
					Write("null");
					break;

				case JsonValueType.Boolean:
					Write(value.AsString);
					break;

				case JsonValueType.Number:
					Write(((double)value).ToString(CultureInfo.InvariantCulture));
					break;

				case JsonValueType.String:
					WriteEncodedString((string)value);
					break;

				case JsonValueType.Object:
					Write(string.Format("JsonObject[{0}]", value.AsJsonObject.Count));
					break;

				case JsonValueType.Array:
					Write(string.Format("JsonArray[{0}]", value.AsJsonArray.Count));
					break;

				default:
					throw new InvalidOperationException("Invalid value type.");
			}
		}

		private void WriteEncodedString(string text)
		{
			Write("\"");

			for (int i = 0; i < text.Length; i += 1)
			{
				var currentChar = text[i];

				// Encoding special characters.
				switch (currentChar)
				{
					case '\\':
						this.writer.Write("\\\\");
						break;

					case '\"':
						this.writer.Write("\\\"");
						break;

					case '/':
						this.writer.Write("\\/");
						break;

					case '\b':
						this.writer.Write("\\b");
						break;

					case '\f':
						this.writer.Write("\\f");
						break;

					case '\n':
						this.writer.Write("\\n");
						break;

					case '\r':
						this.writer.Write("\\r");
						break;

					case '\t':
						this.writer.Write("\\t");
						break;

					default:
						this.writer.Write(currentChar);
						break;
				}
			}

			this.writer.Write("\"");
		}

		private void WriteIndentation()
		{
			for (var i = 0; i < this.indent; i += 1)
			{
				Write(this.IndentString);
			}
		}

		private void WriteSpacing()
		{
			Write(this.SpacingString);
		}

		private void WriteLine()
		{
			Write(this.NewLineString);
			this.isNewLine = true;
		}

		private void WriteLine(string line)
		{
			Write(line);
			WriteLine();
		}

		private void AddRenderingCollection(IEnumerable<JsonValue> value)
		{
			if (!renderingCollections.Add(value))
			{
				throw new JsonSerializationException(ErrorType.CircularReference);
			}
		}

		private void RemoveRenderingCollection(IEnumerable<JsonValue> value)
		{
			renderingCollections.Remove(value);
		}

		private void Render(JsonValue value)
		{
			switch (value.Type)
			{
				case JsonValueType.Null:
				case JsonValueType.Boolean:
				case JsonValueType.Number:
				case JsonValueType.String:
					WriteEncodedJsonValue(value);
					break;

				case JsonValueType.Object:
					Render((JsonObject)value);
					break;

				case JsonValueType.Array:
					Render((JsonArray)value);
					break;

				default:
					throw new JsonSerializationException(ErrorType.InvalidValueType);
			}
		}

		private void Render(JsonArray value)
		{
			AddRenderingCollection(value);

			WriteLine("[");

			indent += 1;

			using (var enumerator = value.GetEnumerator())
			{
				var hasNext = enumerator.MoveNext();

				while (hasNext)
				{
					Render(enumerator.Current);

					hasNext = enumerator.MoveNext();

					if (hasNext)
					{
						WriteLine(",");
					}
					else
					{
						WriteLine();
					}
				}
			}

			indent -= 1;

			Write("]");

			RemoveRenderingCollection(value);
		}

		private void Render(JsonObject value)
		{
			AddRenderingCollection(value);

			WriteLine("{");

			indent += 1;

			using(var enumerator = GetJsonObjectEnumerator(value))
			{
				var hasNext = enumerator.MoveNext();

				while (hasNext)
				{
					WriteEncodedString(enumerator.Current.Key);
					Write(":");
					WriteSpacing();
					Render(enumerator.Current.Value);

					hasNext = enumerator.MoveNext();

					if (hasNext)
					{
						WriteLine(",");
					}
					else
					{
						WriteLine();
					}
				}
			}

			indent -= 1;

			Write("}");

			RemoveRenderingCollection(value);
		}

		/// <summary>
		/// Gets an JsonObject enumarator based on the configuration of this JsonWriter.
		/// If JsonWriter.SortObjects is set to true, then a ordered enumerator is returned.
		/// Otherwise, a faster non-deterministic enumerator is returned.
		/// </summary>
		/// <param name="jsonObject">The JsonObject for which to get an enumerator.</param>
		private IEnumerator<KeyValuePair<string, JsonValue>> GetJsonObjectEnumerator(JsonObject jsonObject)
		{
			if (this.SortObjects)
			{
				var sortedDictionary = new SortedDictionary<string, JsonValue>(StringComparer.Ordinal);

				foreach (var item in jsonObject)
				{
					sortedDictionary.Add(item.Key, item.Value);
				}

				return sortedDictionary.GetEnumerator();
			}
			else
			{
				return jsonObject.GetEnumerator();
			}
		}

		/// <summary>
		/// Returns a string representation of the given JsonValue.
		/// </summary>
		/// <param name="jsonValue">The JsonValue to serialize.</param>
		public string Serialize(JsonValue jsonValue)
		{
			Initialize();

			Render(jsonValue);

			return writer.ToString();
		}

		/// <summary>
		/// Releases all the resources used by this object.
		/// </summary>
		public void Dispose()
		{
			if (this.writer != null)
			{
				this.writer.Dispose();
			}
		}

		private static bool IsValidNumber(double number)
		{
			return !(double.IsNaN(number) || double.IsInfinity(number));
		}
	}
}
