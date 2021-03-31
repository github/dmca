// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// Base class for a <see cref="TableSyntax"/> or a <see cref="TableArraySyntax"/>
    /// </summary>
    public abstract class TableSyntaxBase : SyntaxNode
    {
        private SyntaxToken _openBracket;
        private KeySyntax _name;
        private SyntaxToken _closeBracket;
        private SyntaxToken _endOfLineToken;

        internal TableSyntaxBase(SyntaxKind kind) : base(kind)
        {
            Items = new SyntaxList<KeyValueSyntax>() { Parent = this };
        }

        /// <summary>
        /// Gets or sets the open bracket (simple `[` for <see cref="TableSyntax"/>, double `[[` for <see cref="TableArraySyntax"/>)
        /// </summary>
        public SyntaxToken OpenBracket
        {
            get => _openBracket;
            set => ParentToThis(ref _openBracket, value, OpenTokenKind);
        }

        /// <summary>
        /// Gets or sets the name of this table
        /// </summary>
        public KeySyntax Name
        {
            get => _name;
            set => ParentToThis(ref _name, value);
        }

        /// <summary>
        /// Gets or sets the close bracket (simple `]` for <see cref="TableSyntax"/>, double `]]` for <see cref="TableArraySyntax"/>)
        /// </summary>
        public SyntaxToken CloseBracket
        {
            get => _closeBracket;
            set => ParentToThis(ref _closeBracket, value, CloseTokenKind);
        }

        /// <summary>
        /// Gets the new-line.
        /// </summary>
        public SyntaxToken EndOfLineToken
        {
            get => _endOfLineToken;
            set => ParentToThis(ref _endOfLineToken, value, TokenKind.NewLine, TokenKind.Eof);
        }

        /// <summary>
        /// Gets the key-values associated with this table.
        /// </summary>
        public SyntaxList<KeyValueSyntax> Items { get; }
        
        public override int ChildrenCount => 5;

        internal abstract TokenKind OpenTokenKind { get; }

        internal abstract TokenKind CloseTokenKind { get; }

        protected override SyntaxNode GetChildrenImpl(int index)
        {
            switch (index)
            {
                case 0: return OpenBracket;
                case 1: return Name;
                case 2: return CloseBracket;
                case 3: return EndOfLineToken;
                default:
                    return Items;
            }
        }
    }
}