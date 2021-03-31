// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn.Text
{
    internal struct StringSourceView : ISourceView<StringCharacterIterator>
    {
        private readonly string _text;

        public StringSourceView(string text, string sourcePath)
        {
            this._text = text;
            SourcePath = sourcePath;
        }

        public string SourcePath { get; }

        public string GetString(int offset, int length)
        {
            if (offset + length <= _text.Length)
            {
                return _text.Substring(offset, length);
            }
            return null;
        }

        public StringCharacterIterator GetIterator()
        {
            return new StringCharacterIterator(_text);   
        }

    }
}