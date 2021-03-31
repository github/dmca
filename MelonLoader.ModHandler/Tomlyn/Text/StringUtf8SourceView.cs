// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System.Text;

namespace MelonLoader.Tomlyn.Text
{
    internal struct StringUtf8SourceView: ISourceView<StringCharacterUtf8Iterator>
    {
        private readonly byte[] _text;

        public StringUtf8SourceView(byte[] text, string sourcePath)
        {
            this._text = text;
            SourcePath = sourcePath;
        }

        public string SourcePath { get; }

        public string GetString(int offset, int length)
        {
            if (offset + length <= _text.Length)
            {
                return Encoding.UTF8.GetString(_text, offset, length);
            }
            return null;
        }

        public StringCharacterUtf8Iterator GetIterator()
        {
            return new StringCharacterUtf8Iterator(_text);
        }
    }
}