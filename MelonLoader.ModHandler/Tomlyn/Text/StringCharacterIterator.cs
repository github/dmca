// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System.Runtime.CompilerServices;

namespace MelonLoader.Tomlyn.Text
{
    internal struct StringCharacterIterator : CharacterIterator
    {
        private readonly string _text;

        public StringCharacterIterator(string text)
        {
            this._text = text;
        }

        public int Start => 0;

        public char32? TryGetNext(ref int position)
        {
            if (position < _text.Length)
            {
                var c1 = _text[position];
                position++;

                // Handle surrogates
                return char.IsHighSurrogate(c1) ? NextCharWithSurrogate(ref position, c1) : c1;
            }

            position = _text.Length;
            return null;
        }

        private int NextCharWithSurrogate(ref int position, char c1)
        {
            if (position < _text.Length)
            {
                var c2 = _text[position];
                position++;
                if (char.IsLowSurrogate(c2))
                {
                    return char.ConvertToUtf32(c1, c2);
                }
                throw new CharReaderException("Unexpected character after high-surrogate char");
            }
            throw new CharReaderException("Unexpected EOF after high-surrogate char");
        }
    }
}