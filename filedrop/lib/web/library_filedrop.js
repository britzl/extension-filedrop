// https://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html

var FileDropLibrary = {

    $Context: {
        listener: null
    },

    FileDrop_PlatformSetEventListener: function(listener) {
        Context.listener = listener;

        document.activeElement.addEventListener("dragover", event => {
            if (event.stopPropagation) event.stopPropagation();
            if (event.preventDefault) event.preventDefault();
            event.dataTransfer.dropEffect = 'copy';
            {{{ makeDynCall("viiii", "Context.listener") }}} (
                allocate(intArrayFromString("dragover"), ALLOC_STACK),
                0,
                0,
                0
            );

        }, { passive: false });

        document.activeElement.addEventListener("dragout", event => {
            if (event.stopPropagation) event.stopPropagation();
            if (event.preventDefault) event.preventDefault();
            {{{ makeDynCall("viiii", "Context.listener") }}} (
                allocate(intArrayFromString("dragout"), ALLOC_STACK),
                0,
                0,
                0
            );
        }, { passive: false });

        document.activeElement.addEventListener("drop", event => {
            if (event.stopPropagation) event.stopPropagation();
            if (event.preventDefault) event.preventDefault();
            var files = [];
            if (event.dataTransfer.items) {
                for (var i = 0; i < event.dataTransfer.items.length; i++) {
                    if (event.dataTransfer.items[i].kind === 'file') {
                        var file = event.dataTransfer.items[i].getAsFile();
                        files.push(file);
                    }
                }
            } else {
                files = event.dataTransfer.files;
            }

            for (var i = 0; i < files.length; i++) {
                var file = files[i];
                console.log("Loading file " + file.name);
                const reader = new FileReader();
                reader.addEventListener("load", (event) => {
                    console.log("Loaded file " + file.name, event.target.result);
                    const bufferLength = event.target.result.byteLength;
                    var buffer = _malloc(bufferLength);
                    HEAPU8.set(event.target.result, buffer);
                    {{{ makeDynCall("viiii", "Context.listener") }}} (
                        allocate(intArrayFromString("drop"), ALLOC_STACK),
                        allocate(intArrayFromString(file.name), ALLOC_STACK),
                        buffer,
                        bufferLength
                    );
                    _free(buffer);
                });
                reader.readAsArrayBuffer(file);
            }
        }, { passive: false });
    }
};

autoAddDeps(FileDropLibrary, "$Context");

mergeInto(LibraryManager.library, FileDropLibrary);
