// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// Root TOML syntax tree
    /// </summary>
    public sealed class DocumentSyntax : SyntaxNode
    {
        /// <summary>
        /// Creates an instance of a <see cref="DocumentSyntax"/>
        /// </summary>
        public DocumentSyntax() : base(SyntaxKind.Document)
        {
            KeyValues = new SyntaxList<KeyValueSyntax>() {Parent = this};
            Tables = new SyntaxList<TableSyntaxBase>() { Parent = this };
            Diagnostics = new DiagnosticsBag();
        }

        /// <summary>
        /// Gets the diagnostics attached to this document.
        /// </summary>
        public DiagnosticsBag Diagnostics { get; }

        /// <summary>
        /// Gets a boolean indicating if the <see cref="Diagnostics"/> has any errors.
        /// </summary>
        public bool HasErrors => Diagnostics.HasErrors;

        /// <summary>
        /// Gets the list of <see cref="KeyValueSyntax"/>
        /// </summary>
        public SyntaxList<KeyValueSyntax> KeyValues { get; }

        /// <summary>
        /// Gets the list of tables (either <see cref="TableSyntax"/> or <see cref="TableArraySyntax"/>)
        /// </summary>
        public SyntaxList<TableSyntaxBase> Tables { get; }

        public override void Accept(SyntaxVisitor visitor)
        {
            visitor.Visit(this);
        }
        
        public override int ChildrenCount => 2;

        protected override SyntaxNode GetChildrenImpl(int index)
        {
            return index == 0 ? (SyntaxNode)KeyValues : Tables;
        }
    }
}