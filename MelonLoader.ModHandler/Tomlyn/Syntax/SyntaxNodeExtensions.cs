// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using System.Collections.Generic;

namespace MelonLoader.Tomlyn.Syntax
{
    public static class SyntaxNodeExtensions
    {
        public static void Add(this SyntaxList<KeyValueSyntax> list, string name, int value)
        {
            if (list == null) throw new ArgumentNullException(nameof(list));
            list.Add(new KeyValueSyntax(name, new IntegerValueSyntax(value)));
        }

        public static void Add(this SyntaxList<KeyValueSyntax> list, string name, long value)
        {
            if (list == null) throw new ArgumentNullException(nameof(list));
            list.Add(new KeyValueSyntax(name, new IntegerValueSyntax(value)));
        }

        public static void Add(this SyntaxList<KeyValueSyntax> list, string name, bool value)
        {
            if (list == null) throw new ArgumentNullException(nameof(list));
            list.Add(new KeyValueSyntax(name, new BooleanValueSyntax(value)));
        }

        public static void Add(this SyntaxList<KeyValueSyntax> list, string name, double value)
        {
            if (list == null) throw new ArgumentNullException(nameof(list));
            list.Add(new KeyValueSyntax(name, new FloatValueSyntax(value)));
        }

        public static void Add(this SyntaxList<KeyValueSyntax> list, string name, string value)
        {
            if (list == null) throw new ArgumentNullException(nameof(list));
            list.Add(new KeyValueSyntax(name, new StringValueSyntax(value)));
        }

        public static void Add(this SyntaxList<KeyValueSyntax> list, string name, int[] values)
        {
            if (list == null) throw new ArgumentNullException(nameof(list));
            list.Add(new KeyValueSyntax(name, new ArraySyntax(values)));
        }
        
        public static void Add(this SyntaxList<KeyValueSyntax> list, string name, string[] values)
        {
            if (list == null) throw new ArgumentNullException(nameof(list));
            list.Add(new KeyValueSyntax(name, new ArraySyntax(values)));
        }

        public static void Add(this SyntaxList<KeyValueSyntax> list, string name, DateTimeValueSyntax value)
		{
			if (list == null) throw new ArgumentNullException(nameof(list));
			list.Add(new KeyValueSyntax(name, value));
		}

        public static KeyValueSyntax AddComment(this KeyValueSyntax keyValue, string comment)
        {
            if (keyValue == null) throw new ArgumentNullException(nameof(keyValue));
            if (keyValue.Value == null) throw new InvalidOperationException("The Value must not be null on the KeyValueSyntax");
            keyValue.Value.AddTrailingWhitespace().AddComment(comment);
            return keyValue;
        }

        public static T AddLeadingWhitespace<T>(this T node) where T : SyntaxNode
        {
            if (node == null) throw new ArgumentNullException(nameof(node));
            var trivias = node.LeadingTrivia;
            if (trivias == null)
            {
                trivias = new List<SyntaxTrivia>();
                node.LeadingTrivia = trivias;
            }
            trivias.Add(SyntaxFactory.Whitespace());
            return node;
        }

        public static T AddTrailingWhitespace<T>(this T node) where T : SyntaxNode
        {
            if (node == null) throw new ArgumentNullException(nameof(node));
            var trivias = node.TrailingTrivia;
            if (trivias == null)
            {
                trivias = new List<SyntaxTrivia>();
                node.TrailingTrivia = trivias;
            }
            trivias.Add(SyntaxFactory.Whitespace());
            return node;
        }

        public static T AddComment<T>(this T node, string comment) where T : SyntaxNode
        {
            var trivias = node.TrailingTrivia;
            if (trivias == null)
            {
                trivias = new List<SyntaxTrivia>();
                node.TrailingTrivia = trivias;
            }
            trivias.Add(SyntaxFactory.Comment(comment));
            return node;
        }
    }
}
