// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;

namespace MelonLoader.Tomlyn.Text
{
    internal sealed class CharReaderException : Exception
    {
        public CharReaderException(string message) : base(message)
        {
        }
    }
}