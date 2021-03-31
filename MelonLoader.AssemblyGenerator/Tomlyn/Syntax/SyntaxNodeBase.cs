// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// Base class for <see cref="SyntaxNode"/> and <see cref="SyntaxTrivia"/>
    /// </summary>
    public abstract class SyntaxNodeBase
    {
        /// <summary>
        /// The text source span, read-write, manually updated from children.
        /// </summary>
        public SourceSpan Span;

        /// <summary>
        /// Allow to visit this instance with the specified visitor.
        /// </summary>
        /// <param name="visitor">The visitor</param>
        public abstract void Accept(SyntaxVisitor visitor);

        /// <summary>
        /// Gets the parent of this node.
        /// </summary>
        public SyntaxNode Parent { get; internal set; }
    }
}