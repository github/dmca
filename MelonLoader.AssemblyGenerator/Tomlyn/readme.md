# Tomlyn [![Build Status](https://github.com/xoofx/Tomlyn/workflows/ci/badge.svg?branch=master)](https://github.com/xoofx/Tomlyn/actions) [![NuGet](https://img.shields.io/nuget/v/Tomlyn.svg)](https://www.nuget.org/packages/Tomlyn/)

<img align="right" width="160px" height="160px" src="img/logo.png">

Tomlyn is a TOML parser, validator and authoring library for .NET Framework and .NET Core

## Features

- Very fast parser, GC friendly
- Compatible with latest [TOML 0.5 specs](https://github.com/toml-lang/toml)
- Support perfect load/save roundtrip while preserving all spaces, new line, comments
- Provides a validator with the `Toml.Validate` method
- Provides accurate parsing and validation error messages with precise source location
- Allow to work on the syntax tree directly (preserving styles) through the `Toml.Parse`
- Allow to work with a runtime representation `Toml.ToModel` (but cannot be saved back to TOML)
- Supports for .NET Framework 4.5+, .NET Standard 1.3 and .NET Standard 2.0+ (Core)

## Usage

```C#
var input = @"[mytable]
key = 15
val = true
";

// Gets a syntax tree of the TOML text
var doc = Toml.Parse(input); // returns a DocumentSyntax
// Check for parsing errors with doc.HasErrors and doc.Diagnostics
// doc.HasErrors => throws an exception

// Prints the exact representation of the input
var docStr = doc.ToString();
Console.WriteLine(docStr);

// Gets a runtime representation of the syntax tree
var table = doc.ToModel();
var key = (long) ((TomlTable) table["mytable"])["key"];
var value = (bool) ((TomlTable) table["mytable"])["val"];
Console.WriteLine($"key = {key}, val = {value}");
```

Creates a TOML document programmatically:

```C#
var doc = new DocumentSyntax()
{
    Tables =
    {
        new TableSyntax("test")
        {
            Items =
            {
                {"a", 1},
                {"b", true },
                {"c", "Check"},
                {"d", "ToEscape\nWithAnotherChar\t" },
                {"e", 12.5 },
                {"f", new int[] {1,2,3,4} },
                {"g", new string[] {"0", "1", "2"} },
                {"key with space", 2}
            }
        }
    }
};
Console.WriteLine(doc);
// Prints:
// [test]
// a = 1
// b = true
// c = "Check"
// d = "ToEscape\nWithAnotherChar\t"
// e = 12.5
// f = [1, 2, 3, 4]
// g = ["0", "1", "2"]
// "key with space" = 2
```

## License

This software is released under the [BSD-Clause 2 license](https://opensource.org/licenses/BSD-2-Clause). 

## Credits

Modified version of the logo `Thor` by [Mike Rowe](https://thenounproject.com/itsmikerowe/) from the Noun Project (Creative Commons)

## Author

Alexandre Mutel aka [xoofx](http://xoofx.com).
