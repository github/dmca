// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;

namespace MelonLoader.Tomlyn.Syntax
{
    public sealed class TableSyntax : TableSyntaxBase
    {
        public TableSyntax() : base(SyntaxKind.Table)
        {
        }

        public TableSyntax(string name) : this()
        {
            if (name == null) throw new ArgumentNullException(nameof(name));
            OpenBracket = SyntaxFactory.Token(TokenKind.OpenBracket);
            Name = new KeySyntax(name);
            CloseBracket = SyntaxFactory.Token(TokenKind.CloseBracket);
            EndOfLineToken = SyntaxFactory.NewLine();
        }

        public TableSyntax(KeySyntax name) : this()
        {
            Name = name ?? throw new ArgumentNullException(nameof(name));
            OpenBracket = SyntaxFactory.Token(TokenKind.OpenBracket);
            CloseBracket = SyntaxFactory.Token(TokenKind.CloseBracket);
            EndOfLineToken = SyntaxFactory.NewLine();
        }

        public override void Accept(SyntaxVisitor visitor)
        {
            visitor.Visit(this);
        }

        internal override TokenKind OpenTokenKind => TokenKind.OpenBracket;

        internal override TokenKind CloseTokenKind => TokenKind.CloseBracket;
    }
}