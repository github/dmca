// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;

namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// An array TOML node.
    /// </summary>
    public sealed class ArraySyntax : ValueSyntax
    {
        private SyntaxToken _openBracket;
        private SyntaxToken _closeBracket;

        /// <summary>
        /// Creates an instance of an <see cref="ArraySyntax"/>
        /// </summary>
        public ArraySyntax() : base(SyntaxKind.Array)
        {
            Items = new SyntaxList<ArrayItemSyntax>() { Parent = this };
        }

        /// <summary>
        /// Creates an instance of an <see cref="ArraySyntax"/>
        /// </summary>
        /// <param name="values">An array of integer values</param>
        public ArraySyntax(int[] values) : this()
        {
            if (values == null) throw new ArgumentNullException(nameof(values));
            OpenBracket = SyntaxFactory.Token(TokenKind.OpenBracket);
            CloseBracket = SyntaxFactory.Token(TokenKind.CloseBracket);
            for (int i = 0; i < values.Length; i++)
            {
                var item = new ArrayItemSyntax {Value = new IntegerValueSyntax(values[i])};
                if (i + 1 < values.Length)
                {
                    item.Comma = SyntaxFactory.Token(TokenKind.Comma);
                    item.Comma.AddTrailingWhitespace();
                }
                Items.Add(item);
            }
        }

        /// <summary>
        /// Creates an instance of an <see cref="ArraySyntax"/>
        /// </summary>
        /// <param name="values">An array of string values</param>
        public ArraySyntax(string[] values) : this()
        {
            if (values == null) throw new ArgumentNullException(nameof(values));
            OpenBracket = SyntaxFactory.Token(TokenKind.OpenBracket);
            CloseBracket = SyntaxFactory.Token(TokenKind.CloseBracket);
            for (int i = 0; i < values.Length; i++)
            {
                var item = new ArrayItemSyntax { Value = new StringValueSyntax(values[i]) };
                if (i + 1 < values.Length)
                {
                    item.Comma = SyntaxFactory.Token(TokenKind.Comma);
                    item.Comma.AddTrailingWhitespace();
                }
                Items.Add(item);
            }
        }

        /// <summary>
        /// Gets or sets the open bracket `[` token
        /// </summary>
        public SyntaxToken OpenBracket
        {
            get => _openBracket;
            set => ParentToThis(ref _openBracket, value, TokenKind.OpenBracket);
        }

        /// <summary>
        /// Gets the <see cref="ArrayItemSyntax"/> of this array.
        /// </summary>
        public SyntaxList<ArrayItemSyntax> Items { get; }

        /// <summary>
        /// Gets or sets the close bracket `]` token
        /// </summary>
        public SyntaxToken CloseBracket
        {
            get => _closeBracket;
            set => ParentToThis(ref _closeBracket, value, TokenKind.CloseBracket);
        }

        public override void Accept(SyntaxVisitor visitor)
        {
            visitor.Visit(this);
        }

        public override int ChildrenCount => 3;

        protected override SyntaxNode GetChildrenImpl(int index)
        {
            switch (index)
            {
                case 0:
                    return OpenBracket;
                case 1:
                    return Items;
                default:
                    return CloseBracket;
            }
        }
    }
}