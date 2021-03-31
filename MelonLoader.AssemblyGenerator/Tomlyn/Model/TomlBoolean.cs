// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn.Model
{
    /// <summary>
    /// Runtime representation of a TOML bool
    /// </summary>
    public sealed class TomlBoolean : TomlValue<bool>
    {
        public TomlBoolean(bool value) : base(ObjectKind.Boolean, value)
        {
        }
    }
}