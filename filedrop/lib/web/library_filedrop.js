// https://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html

var FileDropLibrary = {

    $Context: {
        listener: null,
        element: null,
    },
    $Listeners: {
        OnDragOver: function(event) {
            if (!Context.listener) return;
            if (event.stopPropagation) event.stopPropagation();
            if (event.preventDefault) event.preventDefault();
            event.dataTransfer.dropEffect = 'copy';
            {{{ makeDynCall("viiii", "Context.listener") }}} (
                allocate(intArrayFromString("dragover"), ALLOC_STACK),
                0,
                0,
                0
            );
        },
        OnDragLeave: function(event) {
            if (!Context.listener) return;
            if (event.stopPropagation) event.stopPropagation();
            if (event.preventDefault) event.preventDefault();
            {{{ makeDynCall("viiii", "Context.listener") }}} (
                allocate(intArrayFromString("dragleave"), ALLOC_STACK),
                0,
                0,
                0
            );
        },
        OnDrop: function(event) {
            if (!Context.listener) return;
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
                const reader = new FileReader();
                reader.addEventListener("load", (event) => {
                    const bufferLength = event.target.result.byteLength;
                    var buffer = _malloc(bufferLength);
                    const uint8Buffer = new Uint8Array(event.target.result);
                    HEAPU8.set(uint8Buffer, buffer);
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
        },
    },
    FileDrop_PlatformAppInitialize: function() {
        Context.element = document.activeElement;
        document.activeElement.addEventListener("dragover", Listeners.OnDragOver, { passive: false });
        document.activeElement.addEventListener("dragleave", Listeners.OnDragLeave, { passive: false });
        document.activeElement.addEventListener("drop", Listeners.OnDrop, { passive: false });
    },
    FileDrop_PlatformAppFinalize: function() {
        Context.element.removeEventListener("dragover", Listeners.OnDragOver);
        Context.element.removeEventListener("dragleave", Listeners.OnDragLeave);
        Context.element.removeEventListener("drop", Listeners.OnDrop);
    },
    FileDrop_PlatformFinalize: function() {},
    FileDrop_PlatformSetEventListener: function(listener) {
        Context.listener = listener;
    }
};

autoAddDeps(FileDropLibrary, "$Context");
autoAddDeps(FileDropLibrary, "$Listeners");

mergeInto(LibraryManager.library, FileDropLibrary);
