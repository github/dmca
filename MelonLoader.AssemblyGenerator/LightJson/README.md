# LightJson

A minimalist JSON library designed to easily encode and decode JSON messages.

## Features

- Support for .NET Framework 3.5 or later.
- Strict adherence to JSON Standard as defined in [json.org](http://json.org/).
- Expressive fluent API.
- Configurable output (minified/pretty).
- Enhanced debugging data for Visual Studio.

## Basic Usage

### Creating a JSON message

```C#
var json = new JsonObject()
	.Add("menu", new JsonArray()
		.Add("home")
		.Add("projects")
		.Add("about")
	)
	.ToString(true);
```

JSON output:

```JSON
{
	"menu": [
		"home",
		"projects",
		"about"
	]
}
```

### Reading a JSON message

In this example, the variable `json` contains the string generated in the previous example.

```C#
var menu = JsonValue.Parse(json)["menu"].AsJsonArray;

foreach (var item in menu)
{
	Console.WriteLine(item);
}
```

Console output:

```
home
projects
about
```

## License

- MIT License ([Read License](LICENSE.txt)).

## Author

- Marcos López C. (MarcosLopezC) <MarcosLopezC@live.com>
