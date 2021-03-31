// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using MelonLoader.Tomlyn.Model;
using MelonLoader.Tomlyn.Text;

namespace MelonLoader.Tomlyn.Syntax
{
    internal class SyntaxValidator : SyntaxVisitor
    {
        private readonly DiagnosticsBag _diagnostics;
        private ObjectPath _currentPath;
        private readonly Dictionary<ObjectPath, ObjectPathValue> _maps;
        private int _currentArrayIndex;


        public SyntaxValidator(DiagnosticsBag diagnostics)
        {
            _diagnostics = diagnostics ?? throw new ArgumentNullException(nameof(diagnostics));
            _currentPath = new ObjectPath();
            _maps = new Dictionary<ObjectPath, ObjectPathValue>();
        }

        public override void Visit(KeyValueSyntax keyValue)
        {
            var savedPath = _currentPath.Clone();
            if (keyValue.Key == null)
            {
                _diagnostics.Error(keyValue.Span, "A KeyValueSyntax must have a non null Key");
                // returns immediately
                return;
            }

            if (!KeyNameToObjectPath(keyValue.Key, ObjectKind.Table))
            {
                return;
            }

            ObjectKind kind;
            switch (keyValue.Value)
            {
                case ArraySyntax _:
                    kind = ObjectKind.Array;
                    break;
                case BooleanValueSyntax _:
                    kind = ObjectKind.Boolean;
                    break;
                case DateTimeValueSyntax time:
                    switch (time.Kind)
                    {
                        case SyntaxKind.OffsetDateTime:
                            kind = ObjectKind.OffsetDateTime;
                            break;
                        case SyntaxKind.LocalDateTime:
                            kind = ObjectKind.LocalDateTime;
                            break;
                        case SyntaxKind.LocalDate:
                            kind = ObjectKind.LocalDate;
                            break;
                        case SyntaxKind.LocalTime:
                            kind = ObjectKind.LocalTime;
                            break;
                        default:
                            throw new NotSupportedException($"Unsupported datetime kind `{time.Kind}` for the key-value `{keyValue}`");
                    }
                    break;
                case FloatValueSyntax _:
                    kind = ObjectKind.Float;
                    break;
                case InlineTableSyntax _:
                    kind = ObjectKind.Table;
                    break;
                case IntegerValueSyntax _:
                    kind = ObjectKind.Integer;
                    break;
                case StringValueSyntax _:
                    kind = ObjectKind.String;
                    break;
                default:
                    _diagnostics.Error(keyValue.Span, keyValue.Value == null ? $"A KeyValueSyntax must have a non null Value" : $"Not supported type `{keyValue.Value.Kind}` for the value of a KeyValueSyntax");
                    return;
            }
            AddObjectPath(keyValue, kind, false);

            base.Visit(keyValue);
            _currentPath = savedPath;
        }

        public override void Visit(StringValueSyntax stringValue)
        {
            if (stringValue.Token == null)
            {
                _diagnostics.Error(stringValue.Span, $"A StringValueSyntax must have a non null Token");
            }
            base.Visit(stringValue);
        }

        public override void Visit(IntegerValueSyntax integerValue)
        {
            if (integerValue.Token == null)
            {
                _diagnostics.Error(integerValue.Span, $"A IntegerValueSyntax must have a non null Token");
            }
            base.Visit(integerValue);
        }

        public override void Visit(BooleanValueSyntax boolValue)
        {
            if (boolValue.Token == null)
            {
                _diagnostics.Error(boolValue.Span, $"A BooleanValueSyntax must have a non null Token");
            }
            base.Visit(boolValue);
        }

        public override void Visit(FloatValueSyntax floatValue)
        {
            if (floatValue.Token == null)
            {
                _diagnostics.Error(floatValue.Span, $"A FloatValueSyntax must have a non null Token");
            }
            base.Visit(floatValue);
        }

        public override void Visit(TableSyntax table)
        {
            VerifyTable(table);
            var savedPath = _currentPath.Clone();
            if (table.Name == null || !KeyNameToObjectPath(table.Name, ObjectKind.Table))
            {
                return;
            }

            AddObjectPath(table, ObjectKind.Table, false);

            base.Visit(table);

            _currentPath = savedPath;
        }

        public override void Visit(TableArraySyntax table)
        {
            VerifyTable(table);
            var savedPath = _currentPath.Clone();
            if (table.Name == null || !KeyNameToObjectPath(table.Name, ObjectKind.Table))
            {
                return;
            }
            var currentArrayTable = AddObjectPath(table, ObjectKind.TableArray, true);

            var savedIndex = _currentArrayIndex;
            _currentArrayIndex = currentArrayTable.ArrayIndex;

            base.Visit(table);

            currentArrayTable.ArrayIndex++;
            _currentArrayIndex = savedIndex;
            _currentPath = savedPath;
        }

        public override void Visit(BareKeySyntax bareKey)
        {
            if (bareKey.Key == null)
            {
                _diagnostics.Error(bareKey.Span, $"A BareKeySyntax must have a non null property Key");
            }            
            base.Visit(bareKey);
        }

        public override void Visit(KeySyntax key)
        {
            if (key.Key == null)
            {
                _diagnostics.Error(key.Span, $"A KeySyntax must have a non null property Key");
            }
            base.Visit(key);
        }

        public override void Visit(DateTimeValueSyntax dateTime)
        {
            if (dateTime.Token == null)
            {
                _diagnostics.Error(dateTime.Span, $"A DateTimeValueSyntax must have a non null Token");
            }
            base.Visit(dateTime);
        }

        private void VerifyTable(TableSyntaxBase table)
        {
            var isTableArray = table is TableArraySyntax;
            if (table.OpenBracket == null)
            {
                _diagnostics.Error(table.Span, $"The table{(isTableArray? " array" : string.Empty)} must have an {table.OpenTokenKind} `{table.OpenTokenKind.ToText()}`");
            }
            if (table.CloseBracket == null)
            {
                _diagnostics.Error(table.Span, $"The table{(isTableArray ? " array" : string.Empty)} must have an {table.CloseTokenKind} `{table.CloseTokenKind.ToText()}`");
            }
            if (table.EndOfLineToken == null && table.Items.ChildrenCount > 0)
            {
                _diagnostics.Error(table.Span, $"The table{(isTableArray ? " array" : string.Empty)} must have a EndOfLine set after the open/closing brackets and before any elements");
            }
            if (table.Name == null)
            {
                _diagnostics.Error(table.Span, $"The table{(isTableArray ? " array" : string.Empty)} must have a name");
            }
        }

        private bool KeyNameToObjectPath(KeySyntax key, ObjectKind kind)
        {
            if (key.Key == null)
            {
                _diagnostics.Error(key.Span, $"The property KeySyntax.Key cannot be null");
            }

            var name = GetStringFromBasic(key.Key);
            if (string.IsNullOrEmpty(name)) return false;

            _currentPath.Add(name);

            var items = key.DotKeys;
            for (int i = 0; i < items.ChildrenCount; i++)
            {
                AddObjectPath(key, kind, true);
                var dotItem = GetStringFromBasic(items.GetChildren(i).Key);
                if (string.IsNullOrEmpty(dotItem)) return false;
                _currentPath.Add(dotItem);
            }

            return true;
        }

        private ObjectPathValue AddObjectPath(SyntaxNode node, ObjectKind kind, bool isImplicit)
        {
            var currentPath = _currentPath.Clone();
            ObjectPathValue existingValue;
            if (_maps.TryGetValue(currentPath, out existingValue))
            {
                if (!((existingValue.IsImplicit || isImplicit) && (existingValue.Kind == kind || isImplicit && existingValue.Kind == ObjectKind.TableArray && kind == ObjectKind.Table)))
                {
                    _diagnostics.Error(node.Span, $"The element `{node.ToString().TrimEnd('\r','\n').ToPrintableString()}` with the key `{currentPath}` is already defined at {existingValue.Node.Span.Start} with `{existingValue.Node.ToString().TrimEnd('\r', '\n').ToPrintableString()}` and cannot be redefined");
                }
                else if (existingValue.Kind == ObjectKind.TableArray)
                {
                    _currentPath.Add(existingValue.ArrayIndex);
                }
            }
            else
            {
                existingValue = new ObjectPathValue(node, kind, isImplicit);
                _maps.Add(currentPath, existingValue);
            }
            return existingValue;
        }

        private string GetStringFromBasic(BareKeyOrStringValueSyntax value)
        {
            string result;
            if (value is BareKeySyntax basicKey)
            {
                result = basicKey.Key?.Text;
            }
            else
            {
                result = ((StringValueSyntax) value).Value;
            }

            if (string.IsNullOrEmpty(result))
            {
                _diagnostics.Error(value.Span, $"A `{value.Kind}` must have non null/non empty text value");
                return string.Empty;
            }
            return result;
        }

        public override void Visit(ArraySyntax array)
        {
            var savedIndex = _currentArrayIndex;

            if (array.OpenBracket == null)
            {
                _diagnostics.Error(array.Span, $"The array must have an OpenBracket `[`");
            }
            else if (array.CloseBracket == null)
            {
                _diagnostics.Error(array.Span, $"The array must have an CloseBracket `[`");
            }

            var items = array.Items;
            SyntaxKind firstKind = default;
            for(int i = 0; i < items.ChildrenCount; i++)
            {
                var item = items.GetChildren(i);
                var value = item.Value;
                if (i == 0)
                {
                    firstKind = value.Kind;
                }
                else if (firstKind != value.Kind)
                {
                    _diagnostics.Error(value.Span, $"The array item of type `{value.Kind.ToString().ToLowerInvariant()}` doesn't match the type of the first item: `{firstKind.ToString().ToLowerInvariant()}`");
                }

                if (i + 1 < items.ChildrenCount && item.Comma == null)
                {
                    _diagnostics.Error(item.Span, $"The array item [{i}] must have a comma `,`");
                }
            }
            base.Visit(array);

            _currentArrayIndex = savedIndex;
        }

        public override void Visit(InlineTableItemSyntax inlineTableItem)
        {
            base.Visit(inlineTableItem);
        }

        public override void Visit(ArrayItemSyntax arrayItem)
        {
            _currentPath.Add(_currentArrayIndex);

            if (arrayItem.Value == null)
            {
                _diagnostics.Error(arrayItem.Span, $"The array item [{_currentArrayIndex}] must have a non null value");
            }
            base.Visit(arrayItem);
            _currentArrayIndex++;
        }

        public override void Visit(DottedKeyItemSyntax dottedKeyItem)
        {
            base.Visit(dottedKeyItem);
        }

        public override void Visit(InlineTableSyntax inlineTable)
        {
            base.Visit(inlineTable);
        }

        private class ObjectPath : List<ObjectPathItem>
        {
            private int _hashCode;

            public void Add(string key)
            {
                _hashCode = (_hashCode * 397) ^ key.GetHashCode();
                base.Add(new ObjectPathItem(key));
            }

            public void Add(int index)
            {
                _hashCode = (_hashCode * 397) ^ index;
                base.Add(new ObjectPathItem(index));
            }

            public ObjectPath Clone()
            {
                return (ObjectPath) MemberwiseClone();
            }

            public override bool Equals(object obj)
            {
                var other = (ObjectPath) obj;
                if (other.Count != Count) return false;
                if (other._hashCode != _hashCode) return false;
                for (int i = 0; i < Count; i++)
                {
                    if (this[i] != other[i]) return false;
                }
                return true;
            }

            public override int GetHashCode()
            {
                return _hashCode;
            }

            public override string ToString()
            {
                var buffer = new StringBuilder();
                for (int i = 0; i < Count; i++)
                {
                    if (i > 0) buffer.Append('.');
                    buffer.Append(this[i]);
                }
                return buffer.ToString();
            }
        }

        [DebuggerDisplay("{Node} - {Kind}")]
        private class ObjectPathValue
        {
            public ObjectPathValue(SyntaxNode node, ObjectKind kind, bool isImplicit)
            {
                Node = node;
                Kind = kind;
                IsImplicit = isImplicit;
            }


            public readonly SyntaxNode Node;

            public readonly ObjectKind Kind;

            public readonly bool IsImplicit;

            public int ArrayIndex;
        }
        
        private readonly struct ObjectPathItem : IEquatable<ObjectPathItem>
        {
            public ObjectPathItem(string key) : this()
            {
                Key = key;
            }

            public ObjectPathItem(int index) : this()
            {
                Index = index;
            }


            public readonly string Key;

            public readonly int Index;

            public bool Equals(ObjectPathItem other)
            {
                return string.Equals(Key, other.Key) && Index == other.Index;
            }

            public override bool Equals(object obj)
            {
                if (ReferenceEquals(null, obj)) return false;
                return obj is ObjectPathItem other && Equals(other);
            }

            public override int GetHashCode()
            {
                unchecked
                {
                    return ((Key != null ? Key.GetHashCode() : 0) * 397) ^ Index;
                }
            }

            public static bool operator ==(ObjectPathItem left, ObjectPathItem right)
            {
                return left.Equals(right);
            }

            public static bool operator !=(ObjectPathItem left, ObjectPathItem right)
            {
                return !left.Equals(right);
            }

            public override string ToString()
            {
                return Key ?? $"[{Index}]";
            }
        }
    }
}