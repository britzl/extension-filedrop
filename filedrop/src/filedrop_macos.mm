#if defined(DM_PLATFORM_OSX)
#include "filedrop.h"
#include <dmsdk/sdk.h>

#include <AppKit/AppKit.h>

@interface FileDropDelegate: NSView<NSDraggingDestination>
@end

FileDropDelegate *dragView;
OnEventCallback draggingCallback;

void FileDrop_PlatformSetEventListener(OnEventCallback callback) {
    draggingCallback = callback;
    if (dragView == 0)
    {
        NSWindow* window = dmGraphics::GetNativeOSXNSWindow();
        NSRect frame = [window frame];
        dragView = [[FileDropDelegate alloc] initWithFrame:NSMakeRect(0, 0, frame.size.width, frame.size.height)];
        dragView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
        [dragView registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
        [window.contentView addSubview:dragView];
    }
}

void FileDrop_Finalize() {
    if (dragView != 0)
    {
        [dragView removeFromSuperview];
        [dragView release];
        dragView = 0;
    }
}

@implementation FileDropDelegate

-(NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender {
    NSPasteboard *pboard = [sender draggingPasteboard];
    if ( [[pboard types] containsObject:NSFilenamesPboardType] ) {
        NSArray *files = [pboard propertyListForType:NSFilenamesPboardType];
        for (NSString *path in files) {
            NSData *data = [NSData dataWithContentsOfFile:path];
            draggingCallback((const char*)[@"dragover" UTF8String],
            (const char*)[[path lastPathComponent] UTF8String],
            0,
            0);
        }
    }
    return NSDragOperationCopy;
}

-(BOOL)performDragOperation:(id <NSDraggingInfo>)sender {
    NSPasteboard *pboard = [sender draggingPasteboard];
    if ( [[pboard types] containsObject:NSFilenamesPboardType] ) {
        NSArray *files = [pboard propertyListForType:NSFilenamesPboardType];
        for (NSString *path in files) {
            NSData *data = [NSData dataWithContentsOfFile:path];
            draggingCallback((const char*)[@"drop" UTF8String],
                             (const char*)[[path lastPathComponent] UTF8String],
                             (const uint8_t *)[data bytes],
                             [data length]);
        }
    }
    return YES;
}

-(void)draggingExited:(id<NSDraggingInfo>)sender
{
    draggingCallback((const char*)[@"dragleave" UTF8String], 0,0,0);
}

@end

#endif
