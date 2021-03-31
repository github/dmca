// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// Base class for all TOML values.
    /// </summary>
    public abstract class ValueSyntax : SyntaxNode
    {
        internal ValueSyntax(SyntaxKind kind) : base(kind)
        {
        }
    }
}