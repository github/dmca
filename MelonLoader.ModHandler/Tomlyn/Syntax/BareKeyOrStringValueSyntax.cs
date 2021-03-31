// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// Base class for a <see cref="BareKeySyntax"/> or a <see cref="StringValueSyntax"/>
    /// </summary>
    public abstract class BareKeyOrStringValueSyntax : ValueSyntax
    {
        internal BareKeyOrStringValueSyntax(SyntaxKind kind) : base(kind)
        {
        }
    }
}