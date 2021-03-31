// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn
{
    /// <summary>
    /// Options for parsing a TOML string.
    /// </summary>
    public enum TomlParserOptions
    {
        /// <summary>
        /// Parse and validate.
        /// </summary>
        ParseAndValidate = 0,

        /// <summary>
        /// Parse only the document.
        /// </summary>
        ParseOnly = 1,
    }
}