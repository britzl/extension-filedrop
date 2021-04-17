## FileDrop API for Defold
This extension provides a unified, simple to use interface to handle drag and drop of files onto a Defold application. The extension currently only supports HTML5 through the [Drag And Drop API](https://developer.mozilla.org/en-US/docs/Web/API/HTML_Drag_and_Drop_API/File_drag_and_drop).


To use this library in your Defold project, add the following URL to your `game.project` dependencies:

https://github.com/britzl/extension-filedrop/archive/master.zip

It is recommended to use a link to the zip file of a [specific release](https://github.com/britzl/extension-filedrop/releases).

## Example

```lua
function init(self)
	filedrop.set_listener(function(self, event, filename, data)
		if event == "dragover" then
			print("Drag over")
		elseif event == "dragleave" then
			print("Drag leave")
		elseif event == "drop" then
			print("Dropped ", filename, data)
		end
	end
end
```
