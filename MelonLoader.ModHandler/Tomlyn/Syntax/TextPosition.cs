// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;

namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// A position within a text (offset, line column)
    /// </summary>
    public struct TextPosition : IEquatable<TextPosition>
    {
        public static readonly TextPosition Eof = new TextPosition(-1, -1, -1);   

        /// <summary>
        /// Creates a new instance of a <see cref="TextPosition"/>
        /// </summary>
        /// <param name="offset">Offset in the source text</param>
        /// <param name="line">Line number - zero based</param>
        /// <param name="column">Column number - zero based</param>
        public TextPosition(int offset, int line, int column)
        {
            Offset = offset;
            Column = column;
            Line = line;
        }

        /// <summary>
        /// Gets or sets the offset.
        /// </summary>
        public int Offset { get; set; }

        /// <summary>
        /// Gets or sets the column number (zero based)
        /// </summary>
        public int Column { get; set; }

        /// <summary>
        /// Gets or sets the line number (zero based)
        /// </summary>
        public int Line { get; set; }

        public override string ToString()
        {
            return $"({Line+1},{Column+1})";
        }

        public bool Equals(TextPosition other)
        {
            return Offset == other.Offset && Column == other.Column && Line == other.Line;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            return obj is TextPosition && Equals((TextPosition) obj);
        }

        public override int GetHashCode()
        {
            return Offset;
        }

        public static bool operator ==(TextPosition left, TextPosition right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(TextPosition left, TextPosition right)
        {
            return !left.Equals(right);
        }
    }
}