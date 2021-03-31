// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using System.Collections.Generic;
using MelonLoader.Tomlyn.Syntax;
using MelonLoader.Tomlyn.Text;

namespace MelonLoader.Tomlyn.Parsing
{
    /// <summary>
    /// The parser.
    /// </summary>
    internal partial class Parser<TSourceView> where TSourceView : ISourceView
    {
        private readonly ITokenProvider<TSourceView> _lexer;
        private SyntaxTokenValue _previousToken;
        private SyntaxTokenValue _token;
        private bool _hideNewLine;
        private readonly List<SyntaxTrivia> _currentTrivias;
        private TableSyntaxBase _currentTable = null;
        private DiagnosticsBag _diagnostics;

        /// <summary>
        /// Initializes a new instance of the <see cref="Parser{TSourceView}"/> class.
        /// </summary>
        /// <param name="lexer">The lexer.</param>
        /// <exception cref="System.ArgumentNullException"></exception>
        public Parser(ITokenProvider<TSourceView> lexer)
        {
            this._lexer = lexer;
            _currentTrivias = new List<SyntaxTrivia>();
        }

        // private Stack<ScriptNode> Blocks { get; }

        private SourceSpan CurrentSpan => GetSpanForToken(_token);

        public DocumentSyntax Run()
        {
            var doc = new DocumentSyntax();
            _diagnostics = doc.Diagnostics;

            _currentTable = null;
            _hideNewLine = true;
            NextToken();
            while (TryParseTableEntry(out var itemEntry))
            {
                if (itemEntry == null) continue;
                 
                if (itemEntry is TableSyntaxBase table)
                {
                    _currentTable = table;
                    AddToListAndUpdateSpan(doc.Tables, table);
                }
                else if (_currentTable == null)
                {
                    AddToListAndUpdateSpan(doc.KeyValues, (KeyValueSyntax)itemEntry);
                }
                else
                {
                    // Otherwise, we know that we can only have a key-value
                    AddToListAndUpdateSpan(_currentTable.Items, (KeyValueSyntax)itemEntry);
                }
            }

            if (_currentTable != null)
            {
                Close(_currentTable);
                _currentTable = null;
            }
            Close(doc);

            if (_lexer.HasErrors)
            {
                foreach (var lexerError in _lexer.Errors)
                {
                    Log(lexerError);
                }
            }

            return doc;
        }

        private static void AddToListAndUpdateSpan<TSyntaxNode>(SyntaxList<TSyntaxNode> list, TSyntaxNode node) where TSyntaxNode : SyntaxNode
        {
            if (list.ChildrenCount == 0)
            {
                list.Span.FileName = node.Span.FileName;
                list.Span.Start = node.Span.Start;
            }
            else
            {
                list.Span.End = node.Span.End;
            }

            list.Add(node);
        }

        private bool TryParseTableEntry(out SyntaxNode nextEntry)
        {
            nextEntry = null;
            while (true)
            {
                switch (_token.Kind)
                {
                    case TokenKind.Eof:
                        return false;
                    case TokenKind.BasicKey:
                    case TokenKind.String:
                    case TokenKind.StringLiteral:
                        nextEntry = ParseKeyValue(true);
                        return true;
                    case TokenKind.OpenBracket:
                    case TokenKind.OpenBracketDouble:
                        nextEntry = ParseTableOrTableArray();
                        return true;
                    default:
                        LogError($"Unexpected token [{ToPrintable(_token)}] found");
                        NextToken();
                        break;
                }
            }
        }

        private KeyValueSyntax ParseKeyValue(bool expectEndOfLine)
        {
            // When parsing a key = value, we don't expect NewLines, so we don't hide them as trivia
            var previousState = _hideNewLine;
            _hideNewLine = false;
            try
            {
                var keyValueSyntax = Open<KeyValueSyntax>();
                keyValueSyntax.Key = ParseKey();

                if (_token.Kind != TokenKind.Equal)
                {
                    LogError($"Expecting `=` after a key instead of {ToPrintable(_token)}");
                    Close(keyValueSyntax);
                    // We recover the parsing on the next line
                    SkipAfterEndOfLine();
                }
                else
                {
                    // Switch the lexer to value parser
                    _lexer.State = LexerState.Value;
                    try
                    {
                        keyValueSyntax.EqualToken = EatToken();
                        keyValueSyntax.Value = ParseValue();
                    }
                    finally
                    {
                        _lexer.State = LexerState.Key;
                    }

                    if (expectEndOfLine && _token.Kind != TokenKind.Eof)
                    {
                        keyValueSyntax.EndOfLineToken = EatToken(TokenKind.NewLine);
                    }

                    Close(keyValueSyntax);
                }
                return keyValueSyntax;
            }
            finally
            {
                _hideNewLine = previousState;
            }
        }

        private ValueSyntax ParseValue()
        {
            switch (_token.Kind)
            {
                case TokenKind.Integer:
                case TokenKind.IntegerHexa:
                case TokenKind.IntegerOctal:
                case TokenKind.IntegerBinary:
                    return ParseInteger();

                case TokenKind.Nan:
                case TokenKind.PositiveNan:
                case TokenKind.NegativeNan:
                case TokenKind.Infinite:
                case TokenKind.PositiveInfinite:
                case TokenKind.NegativeInfinite:
                case TokenKind.Float:
                    return ParseFloat();

                case TokenKind.String:
                case TokenKind.StringMulti:
                case TokenKind.StringLiteral:
                case TokenKind.StringLiteralMulti:
                    return ParseString();

                case TokenKind.OpenBracket:
                    return ParseArray();

                case TokenKind.OpenBrace:
                    return ParseInlineTable();

                case TokenKind.OffsetDateTime:
                case TokenKind.LocalDateTime:
                case TokenKind.LocalDate:
                case TokenKind.LocalTime:
                    return ParseDateTime();

                case TokenKind.True:
                case TokenKind.False:
                    return ParseBoolean();

                case TokenKind.NewLine:
                    // Provide a dedicated error for end-of-line
                    // We don't eat the token as it is supposed to be taken by the caller
                    LogError($"Unexpected token end-of-line found while expecting a value");
                    break;

                default:
                    LogError($"Unexpected token `{ToPrintable(_token)}` for a value");
                    // Skip the token as we don't want to loop forever
                    NextToken();
                    break;
            }
            return null;
        }

        private bool IsCurrentValue()
        {
            switch (_token.Kind)
            {
                case TokenKind.Integer:
                case TokenKind.IntegerHexa:
                case TokenKind.IntegerOctal:
                case TokenKind.IntegerBinary:
                case TokenKind.Infinite:
                case TokenKind.PositiveInfinite:
                case TokenKind.NegativeInfinite:
                case TokenKind.Float:
                case TokenKind.String:
                case TokenKind.StringMulti:
                case TokenKind.StringLiteral:
                case TokenKind.StringLiteralMulti:
                case TokenKind.OpenBracket:
                case TokenKind.OpenBrace:
                case TokenKind.OffsetDateTime:
                case TokenKind.LocalDateTime:
                case TokenKind.LocalDate:
                case TokenKind.LocalTime:
                case TokenKind.True:
                case TokenKind.False:
                    return true;
            }
            return false;
        }

        private BooleanValueSyntax ParseBoolean()
        {
            var boolean = Open<BooleanValueSyntax>();
            boolean.Value = (bool)_token.Value;
            boolean.Token = EatToken();            
            return Close(boolean);
        }

        private DateTimeValueSyntax ParseDateTime()
        {
            DateTimeValueSyntax datetime;

            switch (_token.Kind)
            {
                case TokenKind.OffsetDateTime:
                    datetime = Open(new DateTimeValueSyntax(SyntaxKind.OffsetDateTime));
                    break;
                case TokenKind.LocalDateTime:
                    datetime = Open(new DateTimeValueSyntax(SyntaxKind.LocalDateTime));
                    break;
                case TokenKind.LocalDate:
                    datetime = Open(new DateTimeValueSyntax(SyntaxKind.LocalDate));
                    break;
                case TokenKind.LocalTime:
                    datetime = Open(new DateTimeValueSyntax(SyntaxKind.LocalTime));
                    break;
                default:
                    throw new InvalidOperationException("The datetime kind `{_token.Kind}` is not supported");
            }

            datetime.Value = (DateTime)_token.Value;
            datetime.Token = EatToken();
            return Close(datetime);
        }

        private IntegerValueSyntax ParseInteger()
        {
            var i64 = Open<IntegerValueSyntax>();
            i64.Value = (long)_token.Value;
            i64.Token = EatToken();            
            return Close(i64);
        }

        private FloatValueSyntax ParseFloat()
        {
            var f64 = Open<FloatValueSyntax>();
            f64.Value = (double)_token.Value;
            f64.Token = EatToken();
            return Close(f64);
        }

        private ArraySyntax ParseArray()
        {
            var array = Open<ArraySyntax>();
            var saveHideNewLine = _hideNewLine;
            _hideNewLine = true;
            array.OpenBracket = EatToken(TokenKind.OpenBracket);
            try
            {
                bool expectingEndOfArray = false;
                while (true)
                {
                    if (_token.Kind == TokenKind.CloseBracket)
                    {
                        // Before parsing the next token we need to restore the parsing of new line
                        _hideNewLine = saveHideNewLine;
                        array.CloseBracket = EatToken();
                        break;
                    }

                    if (!expectingEndOfArray)
                    {
                        var item = Open<ArrayItemSyntax>();
                        item.Value = ParseValue();

                        if (_token.Kind == TokenKind.Comma)
                        {
                            item.Comma = EatToken();
                        }
                        else if (IsCurrentValue())
                        {
                            LogError($"Missing a `,` (token: comma) to separate items in an array");
                        }
                        else
                        {
                            expectingEndOfArray = true;
                        }
                        Close(item);

                        AddToListAndUpdateSpan(array.Items, item);
                    }
                    else
                    {
                        LogError($"Unexpected token `{ToPrintable(_token)}` (token: `{_token.Kind}`). Expecting a closing `]` for an array");
                        break;
                    }
                }
            }
            finally
            {
                _hideNewLine = saveHideNewLine;
            }
            return Close(array);
        }

        private InlineTableSyntax ParseInlineTable()
        {
            var inlineTable = Open<InlineTableSyntax>();

            var previousState = _lexer.State;
            _lexer.State = LexerState.Key;
            var previousLine = _hideNewLine;
            _hideNewLine = false;
            inlineTable.OpenBrace = EatToken(TokenKind.OpenBrace);
            try
            {

                // toml-specs: Inline tables are intended to appear on a single line. 
                // -> So we don't hide newlines

                bool expectingEndOfInitializer = false;
                while (true)
                {
                    if (_token.Kind == TokenKind.CloseBrace)
                    {
                        _hideNewLine = previousLine;
                        _lexer.State = previousState;
                        inlineTable.CloseBrace = EatToken();
                        break;
                    }

                    if (!expectingEndOfInitializer && (_token.Kind == TokenKind.BasicKey || _token.Kind == TokenKind.String))
                    {
                        var item = Open<InlineTableItemSyntax>();
                        item.KeyValue = ParseKeyValue(false);

                        if (_token.Kind == TokenKind.Comma)
                        {
                            item.Comma = EatToken();
                        }
                        else
                        {
                            expectingEndOfInitializer = true;
                        }

                        Close(item);

                        AddToListAndUpdateSpan(inlineTable.Items, item);
                    }
                    else
                    {
                        LogError($"Unexpected token `{_token.Kind}` while parsing inline table. Expecting a bare key or string instead of `{ToPrintable(_token)}`");
                        break;
                    }
                }
            }
            finally
            {
                _lexer.State = previousState;
                _hideNewLine = previousLine;
            }

            return Close(inlineTable);
        }

        private TableSyntaxBase ParseTableOrTableArray()
        {
            // If we have a pending table, close it
            if (_currentTable != null)
            {
                Close(_currentTable);
            }
            bool isTableArray = _token.Kind == TokenKind.OpenBracketDouble;

            var previousState = _hideNewLine;
            _hideNewLine = false;
            var table = isTableArray ? (TableSyntaxBase)Open<TableArraySyntax>() : Open<TableSyntax>();
            try
            {
                table.OpenBracket = EatToken();
                table.Name = ParseKey();
                table.CloseBracket = EatToken(isTableArray ? TokenKind.CloseBracketDouble : TokenKind.CloseBracket);

                if (_token.Kind != TokenKind.Eof)
                {
                    table.EndOfLineToken = EatToken(TokenKind.NewLine);
                }
                // We don't close the table as it is going to be the new table
            }
            finally
            {
                _hideNewLine = previousState;
            }

            return table;
        }

        private KeySyntax ParseKey()
        {
            var key = Open<KeySyntax>();
            key.Key = ParseBaseKey();
            while (_token.Kind == TokenKind.Dot)
            {
                AddToListAndUpdateSpan(key.DotKeys, ParseDotKey());
            }
            return Close(key);
        }

        private BareKeyOrStringValueSyntax ParseBaseKey()
        {
            if (_token.Kind == TokenKind.BasicKey)
            {
                return ParseBasicKey();
            }

            if (_token.Kind == TokenKind.String || _token.Kind == TokenKind.StringLiteral)
            {
                return ParseString();
            }

            LogError($"Unexpected token `{ToPrintable(_token)}` for a base key");
            NextToken();
            return null;
        }
        
        private StringValueSyntax ParseString()
        {
            var str = Open<StringValueSyntax>();
            str.Value = (string)_token.Value;
            str.Token = EatToken();            
            return Close(str);
        }
        private DottedKeyItemSyntax ParseDotKey()
        {
            var dotKey = Open<DottedKeyItemSyntax>();
            dotKey.Dot = EatToken();
            dotKey.Key = ParseBaseKey();            
            return Close(dotKey);
        }

        private BareKeySyntax ParseBasicKey()
        {
            var basicKey = Open<BareKeySyntax>();
            basicKey.Key = EatToken(TokenKind.BasicKey);
            return Close(basicKey);
        }

        private SyntaxToken EatToken(TokenKind tokenKind)
        {
            SyntaxToken syntax;
            if (_token.Kind == tokenKind)
            {
                syntax = Open<SyntaxToken>();
            }
            else
            {
                // Create an invalid token in case we don't match it
                var invalid = Open<InvalidSyntaxToken>();
                invalid.InvalidKind = _token.Kind;
                syntax = invalid;
                var tokenText = tokenKind.ToText();
                var expectingTokenText = tokenText != null ? $"while expecting `{tokenText}` (token: `{tokenKind.ToString().ToLowerInvariant()}`)" : $"while expecting token `{tokenKind.ToString().ToLowerInvariant()}`";
                if (_token.Kind == TokenKind.Invalid)
                {
                    LogError($"Unexpected token found `{ToPrintable(_token)}` {expectingTokenText}");
                }
                else
                {
                    LogError($"Unexpected token found `{ToPrintable(_token)}` (token: `{_token.Kind.ToString().ToLowerInvariant()}`) {expectingTokenText}");
                }
            }
            syntax.TokenKind = tokenKind;
            syntax.Text = _token.Kind.ToText() ?? _token.GetText(_lexer.Source);
            if (tokenKind == TokenKind.NewLine)
            {
                // Once we have found a new line, we let all the other NewLines as trivias
                _hideNewLine = true;
            }
            NextToken();
            return Close(syntax);
        }

        private SyntaxToken EatToken()
        {
            var syntax = Open<SyntaxToken>();
            syntax.TokenKind = _token.Kind;
            syntax.Text = _token.Kind.ToText() ?? _token.GetText(_lexer.Source);
            NextToken();            
            return Close(syntax);
        }

        private void SkipAfterEndOfLine()
        {
            while (!IsEolOrEof())
            {
                NextToken();
            }
            if (_token.Kind != TokenKind.Eof)
            {
                NextToken();
            }
        }

        private bool IsEolOrEof()
        {
            return _token.Kind == TokenKind.NewLine || _token.Kind == TokenKind.Eof;
        }

        private T Open<T>() where T : SyntaxNode, new()
        {
            return Open<T>(_token);
        }

        private T Open<T>(T syntax) where T : SyntaxNode
        {
            return Open(syntax, _token);
        }

        private T Open<T>(T syntax, SyntaxTokenValue startToken) where T : SyntaxNode
        {
            syntax.Span = new SourceSpan(_lexer.Source.SourcePath, startToken.Start, new TextPosition());

            if (_currentTrivias.Count > 0)
            {
                syntax.LeadingTrivia = new List<SyntaxTrivia>(_currentTrivias);
                _currentTrivias.Clear();
            }
            return syntax;
        }

        private T Open<T>(SyntaxTokenValue startToken) where T : SyntaxNode, new()
        {
            return Open(new T(), startToken);
        }

        private T Close<T>(T syntax) where T : SyntaxNode
        {
            syntax.Span.End = _previousToken.End;
            
            if (_currentTrivias.Count > 0)
            {
                syntax.TrailingTrivia = new List<SyntaxTrivia>(_currentTrivias);
                _currentTrivias.Clear();
            }
            return syntax;
        }

        private string ToPrintable(SyntaxTokenValue localToken)
        {
            return CharHelper.ToPrintableString(ToText(localToken));
        }

        private string ToText(SyntaxTokenValue localToken)
        {
            return localToken.GetText(_lexer.Source);
        }

        private string ToPrintable(SourceSpan span)
        {
            return CharHelper.ToPrintableString(_lexer.Source.GetString(span.Offset, span.Length));
        }

        private void NextToken()
        {
            _previousToken = _token;
            bool result;

            // Skip trivias
            while ((result = _lexer.MoveNext()) && IsHidden(_lexer.Token.Kind))
            {
                _currentTrivias.Add(new SyntaxTrivia { Span = new SourceSpan(_lexer.Source.SourcePath, _lexer.Token.Start, _lexer.Token.End), Kind = _lexer.Token.Kind, Text = _lexer.Token.GetText(_lexer.Source) });
            }

            _token = result ? _lexer.Token : new SyntaxTokenValue(TokenKind.Eof, new TextPosition(), new TextPosition());
        }

        private bool IsHidden(TokenKind tokenKind)
        {
            return tokenKind == TokenKind.Whitespaces || tokenKind == TokenKind.Comment || (tokenKind == TokenKind.NewLine && _hideNewLine);
        }

        private void LogError(string text)
        {
            LogError(_token, text);
        }

        private void LogError(SyntaxTokenValue tokenArg, string text)
        {
            LogError(GetSpanForToken(tokenArg), text);
        }
        
        //private void LogError<T>(SyntaxValueNode<T> tokenArg, string text)
        //{
        //    LogError(tokenArg.Token, text);
        //}

        private SourceSpan GetSpanForToken(SyntaxTokenValue tokenArg)
        {
            return new SourceSpan(_lexer.Source.SourcePath, tokenArg.Start, tokenArg.End);
        }

        private void LogError(SourceSpan span, string text)
        {
            Log(new DiagnosticMessage(DiagnosticMessageKind.Error, span, text));
        }

        private void LogError(SyntaxNode node, string message)
        {
            LogError(node, node.Span, message);
        }

        private void LogError(SyntaxNode node, SourceSpan span, string message)
        {
            LogError(span, $"Error while parsing {node.GetType().Name}: {message}");
        }

        private void Log(DiagnosticMessage diagnosticMessage)
        {
            if (diagnosticMessage == null) throw new ArgumentNullException(nameof(diagnosticMessage));
            _diagnostics.Add(diagnosticMessage);
        }
    }
}