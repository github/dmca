// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System.Collections.Generic;

namespace MelonLoader.Tomlyn.Syntax
{
    public abstract class SyntaxVisitor
    {
        public virtual void Visit(SyntaxList list)
        {
            DefaultVisit(list);
        }
        public virtual void Visit(DocumentSyntax document)
        {
            DefaultVisit(document);
        }
        public virtual void Visit(KeyValueSyntax keyValue)
        {
            DefaultVisit(keyValue);
        }
        public virtual void Visit(StringValueSyntax stringValue)
        {
            DefaultVisit(stringValue);
        }
        public virtual void Visit(IntegerValueSyntax integerValue)
        {
            DefaultVisit(integerValue);
        }
        public virtual void Visit(BooleanValueSyntax boolValue)
        {
            DefaultVisit(boolValue);
        }
        public virtual void Visit(FloatValueSyntax floatValue)
        {
            DefaultVisit(floatValue);
        }
        public virtual void Visit(TableSyntax table)
        {
            DefaultVisit(table);
        }

        public virtual void Visit(TableArraySyntax table)
        {
            DefaultVisit(table);
        }

        public virtual void Visit(SyntaxToken token)
        {
            DefaultVisit(token);
        }
        public virtual void Visit(SyntaxTrivia trivia)
        {
        }
        public virtual void Visit(BareKeySyntax bareKey)
        {
            DefaultVisit(bareKey);
        }
        public virtual void Visit(KeySyntax key)
        {
            DefaultVisit(key);
        }
        public virtual void Visit(DateTimeValueSyntax dateTime)
        {
            DefaultVisit(dateTime);
        }
        
        public virtual void Visit(ArraySyntax array)
        {
            DefaultVisit(array);
        }
        public virtual void Visit(InlineTableItemSyntax inlineTableItem)
        {
            DefaultVisit(inlineTableItem);
        }
        public virtual void Visit(ArrayItemSyntax arrayItem)
        {
            DefaultVisit(arrayItem);
        }
        public virtual void Visit(DottedKeyItemSyntax dottedKeyItem)
        {
            DefaultVisit(dottedKeyItem);
        }
        public virtual void Visit(InlineTableSyntax inlineTable)
        {
            DefaultVisit(inlineTable);
        }

        public virtual void DefaultVisit(SyntaxNode node)
        {
            if (node == null) return;

            VisitTrivias(node.LeadingTrivia);

            for (int i = 0; i < node.ChildrenCount; i++)
            {
                var child = node.GetChildren(i);
                if (child != null)
                {
                    child.Accept(this);
                }
            }

            VisitTrivias(node.TrailingTrivia);
        }

        private void VisitTrivias(List<SyntaxTrivia> trivias)
        {
            if (trivias != null)
            {
                foreach (var trivia in trivias)
                {
                    if (trivia != null)
                    {
                        trivia.Accept(this);
                    }
                }
            }
        }
    }
}