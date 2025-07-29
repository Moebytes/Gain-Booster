#import <Cocoa/Cocoa.h>
#import "NativeMenuBridge.h"

static std::function<void(int)> endCallback = nullptr;

@interface MenuDelegate : NSObject 
@end

@implementation MenuDelegate
- (void) menuItemSelected: (id) sender {
    NSMenuItem* item = (NSMenuItem*) sender;
    int tag = (int) item.tag;
    if (endCallback) {
        endCallback(tag);
    }
    endCallback = nullptr;
}
@end

void showNativeMacMenu(const std::map<int, std::string>& items, std::function<void(int)> callback) {
    endCallback = callback;

    NSMenu* menu = [[NSMenu alloc] initWithTitle: @"Preset Menu"];
    MenuDelegate* delegate = [[MenuDelegate alloc] init];

    for (const auto& pair : items) {
        NSString* title = [NSString stringWithUTF8String:pair.second.c_str()];

        NSMenuItem* menuItem = [[NSMenuItem alloc] initWithTitle: title
                action: @selector(menuItemSelected:) keyEquivalent: @""];

        menuItem.target = delegate;
        menuItem.tag = pair.first;
        [menu addItem:menuItem];
    }

    NSPoint mouseLocation = [NSEvent mouseLocation];
    const CGFloat estimatedMenuHeight = items.size() * 22.0;
    const CGFloat estimatedMenuWidth = 110.0;

    mouseLocation.y += estimatedMenuHeight;
    mouseLocation.x -= estimatedMenuWidth / 2.0;

    [menu popUpMenuPositioningItem: nil atLocation:mouseLocation inView: nil];
}
