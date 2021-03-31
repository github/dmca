## Description

TinyJSON is a simple JSON library for C# that strives for ease of use.

## Features

* Transmogrify objects into JSON and back again.
* Uses reflection to dump and load object graphs automagically.
* Supports primitives, classes, structs, enums, lists, dictionaries and arrays.
* Supports single dimensional arrays, multidimensional arrays and jagged arrays.
* Parsed data uses proxy variants that can be implicitly cast to primitive types for cleaner code, or directly encoded back to JSON.
* Numeric types are handled without fuss.
* Polymorphic classes supported with a type hint encoded into the JSON.
* Supports optionally pretty printing JSON output.
* Supports optionally encode properties and private fields.
* Supports decoding fields and properties from aliased names.
* Unit tested.

## Usage

The API is namespaced under `TinyJSON` and the primary class is `JSON`. There are really only a few methods you need to know:

```csharp
namespace TinyJSON
{
	public static class JSON
	{
		public static Variant Load( string json );
		public static string Dump( object data, EncodeOptions = EncodeOptions.None );
		public static void MakeInto<T>( Variant data, out T item );
	}
}
```

`Load()` will load a string of JSON, returns `null` if invalid or a `Variant` proxy object if successful. The proxy allows for implicit casts and can convert between various C# numeric value types.

```csharp
var data = JSON.Load( "{\"foo\": 1, \"bar\": 2.34}" );
int i = data["foo"];
float f = data["bar"];
```

`Dump()` will take a C# object, list, dictionary or primitive value type and turn it into JSON.

```csharp
var data = new List<int>() { { 0 }, { 1 }, { 2 } };
Console.WriteLine( JSON.Dump( data ) ); // output: [1,2,3]
```

TinyJSON can also handle classes, structs, enums and nested objects. Given these definitions:

```csharp
enum TestEnum
{
	Thing1,
	Thing2,
	Thing3
}


struct TestStruct
{
	public int x;
	public int y;
}


class TestClass
{
	public string name;
	public TestEnum type;
	public List<TestStruct> data = new List<TestStruct>();

	[Exclude]
	public int _ignored;

	[BeforeEncode]
	public void BeforeEncode()
	{
		Console.WriteLine( "BeforeEncode callback fired!" );
	}

	[AfterDecode]
	public void AfterDecode()
	{
		Console.WriteLine( "AfterDecode callback fired!" );
	}
}
```

The following code:

```csharp
var testClass = new TestClass();
testClass.name = "Rumpelstiltskin Jones";
testClass.type = TestEnum.Thing2;
testClass.data.Add( new TestStruct() { x = 1, y = 2 } );
testClass.data.Add( new TestStruct() { x = 3, y = 4 } );
testClass.data.Add( new TestStruct() { x = 5, y = 6 } );

var testClassJson = JSON.Dump( testClass, true );
Console.WriteLine( testClassJson );
```

Will output:

```json
{
	"name": "Rumpelstiltskin Jones",
	"type": "Thing2",
	"data": [
		{
			"x": 1,
			"y": 2
		},
		{
			"x": 3,
			"y": 4
		},
		{
			"x": 5,
			"y": 6
		}
	]
}
```

You can use, `MakeInto()` can be used to reconstruct JSON data back into an object:

```csharp
TestClass testClass;
JSON.MakeInto( JSON.Load( testClassJson ), out testClass );
```

There are also `Make()` methods on `Variant` which provide options for slightly more natural syntax:

```csharp
TestClass testClass;

JSON.Load( json ).Make( out testClass );
// or
testClass = JSON.Load( json ).Make<Data>();
```

Finally, you'll notice that `TestClass` has the methods `BeforeEncode()` and `AfterDecode()` which have the `TinyJSON.BeforeEncode` and `TinyJSON.AfterDecode` attributes. These methods will be called *before* the object starts being serialized and *after* the object has been fully deserialized. This is useful when some further preparation or initialization logic is required.

By default, only public fields are encoded, not properties or private fields. You can tag any field or property to be included with the `TinyJSON.Include` attribute, or force a public field to be excluded with the `TinyJSON.Exclude` attribute.


## Decode Aliases

Fields and properties can be decoded from aliases using the `TinyJSON.DecodeAlias` attribute. While decoding, if no matching data is found in the JSON for a given field or property, its aliases will also be searched for.

```csharp
class TestClass
{
	[DecodeAlias("anotherName")]
	public string name; // decode from "name" or "anotherName"

	[DecodeAlias("anotherNumber", "yetAnotherNumber")]
	public int number; // decode from "number", "anotherNumber", or "yetAnotherNumber"
}
```


## Type Hinting

When decoding polymorphic types, TinyJSON has no way of knowing which subclass to instantiate unless a type hint is included. So, by default, TinyJSON will add a key named `@type` to each encoded object with the fully qualified type of the object.

## Encode Options

Several options are currently available for JSON encoding, and can be passed in as a second parameter to `JSON.Dump()`.

* `EncodeOptions.PrettyPrint` will output nicely formatted JSON to make it more readable.
* `EncodeOptions.NoTypeHints` will disable the outputting of type hints into the JSON output. This may be desirable if you plan to read the JSON into another application that might choke on the type information. You can override this on a per-member basis with the `TinyJSON.TypeHint` attribute.
* `EncodeOptions.IncludePublicProperties` will include public properties in the output.
* `EncodeOptions.EnforceHeirarchyOrder` will ensure fields and properties are encoded in class heirarchy order, from the root base class on down, but comes at a slight performance cost.

## Using Variants

For most use cases you can just assign, cast or make your object graph using the API outlined above, but at times you may need to work with the intermediate proxy objects to, say, dig through and iterate over a collection. To do this, cast the `Variant` to the appropriate subclass (likely either `ProxyArray` or `ProxyObject`) and you're good to go:

```csharp
var list = JSON.Load( "[1,2,3]" );
foreach (var item in list as ProxyArray)
{
	int number = item;
	Console.WriteLine( number );
}

var dict = JSON.Load( "{\"x\":1,\"y\":2}" );
foreach (var pair in dict as ProxyObject)
{
	float value = pair.Value;
	Console.WriteLine( pair.Key + " = " + value );
}
```

The non-collection `Variant` subclasses are `ProxyBoolean`, `ProxyNumber` and `ProxyString`. A variant can also be `null`.

Any `Variant` object can be directly encoded to JSON by calling its `ToJSON()` method or passing it to `JSON.Dump()`.

## Notes

This project was developed with pain elimination and lightweight size in mind. It should be able to handle reasonable amounts of reasonable data at reasonable speeds, but it's not meant for massive data sets.

The primary use case for this library is with Unity3D, so compatibility is focused there, though it should work with most modern C# environments.

It has been used in several published games. It's good for preferences, level and progress data, etc.

## Meta

Handcrafted by Patrick Hogan [[twitter](http://twitter.com/pbhogan) &bull; [github](http://github.com/pbhogan) &bull; [website](http://www.gallantgames.com)]

Based on [MiniJSON](https://gist.github.com/darktable/1411710) by Calvin Rien

Released under the [MIT License](http://www.opensource.org/licenses/mit-license.php).

