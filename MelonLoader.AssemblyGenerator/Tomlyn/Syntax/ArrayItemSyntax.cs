// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// An item of an <see cref="ArraySyntax"/>
    /// </summary>
    public sealed class ArrayItemSyntax : SyntaxNode
    {
        private ValueSyntax _value;
        private SyntaxToken _comma;

        /// <summary>
        /// Creates an instance of <see cref="ArrayItemSyntax"/>
        /// </summary>
        public ArrayItemSyntax() : base(SyntaxKind.ArrayItem)
        {
        }

        /// <summary>
        /// Gets or sets the value of this item.
        /// </summary>
        public ValueSyntax Value
        {
            get => _value;
            set => ParentToThis(ref _value, value);
        }

        /// <summary>
        /// Gets or sets the comma of this item (mandatory to separate elements in an array)
        /// </summary>
        public SyntaxToken Comma
        {
            get => _comma;
            set => ParentToThis(ref _comma, value, TokenKind.Comma);
        }

        public override void Accept(SyntaxVisitor visitor)
        {
            visitor.Visit(this);
        }

        public override int ChildrenCount => 2;

        protected override SyntaxNode GetChildrenImpl(int index)
        {
            return index == 0 ? (SyntaxNode)Value : Comma;
        }
    }
}