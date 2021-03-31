// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// A textual source span.
    /// </summary>
    public struct SourceSpan
    {
        /// <summary>
        /// Creates a source span.
        /// </summary>
        /// <param name="fileName"></param>
        /// <param name="start"></param>
        /// <param name="end"></param>
        public SourceSpan(string fileName, TextPosition start, TextPosition end)
        {
            FileName = fileName;
            Start = start;
            End = end;
        }

        /// <summary>
        /// Gets or sets the filename.
        /// </summary>
        public string FileName;

        /// <summary>
        /// Gets the starting offset of this span.
        /// </summary>
        public int Offset => Start.Offset;

        /// <summary>
        /// Gets the length of this span.
        /// </summary>
        public int Length => End.Offset - Start.Offset + 1;

        /// <summary>
        /// Gets or sets the starting text position.
        /// </summary>
        public TextPosition Start;

        /// <summary>
        /// Gets or sets the ending text position.
        /// </summary>
        public TextPosition End;

        public override string ToString()
        {
            return $"{FileName}{Start}-{End}";
        }

        /// <summary>
        /// A string representation of this source span not including the <see cref="End"/> position.
        /// </summary>
        public string ToStringSimple()
        {
            return $"{FileName}{Start}";
        }
    }
}