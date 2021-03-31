// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// Represents an invalid <see cref="SyntaxToken"/>
    /// </summary>
    public sealed class InvalidSyntaxToken : SyntaxToken
    {
        /// <summary>
        /// The kind of token which is invalid for the context.
        /// </summary>
        public TokenKind InvalidKind { get; set; }
    }
}