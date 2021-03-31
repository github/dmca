// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn.Text
{
    internal interface IStringView
    {
        string GetString(int offset, int length);
    }

    internal interface IStringView<out TCharIterator> : IStringView where TCharIterator : struct, CharacterIterator
    {
        TCharIterator GetIterator();
    }

}