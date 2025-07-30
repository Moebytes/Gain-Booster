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

void showNativeMacMenu(const std::map<int, std::string>& items, 
    const std::map<int, std::string>& factoryItems,
    const std::map<int, std::string>& userItems, const std::string& userFolder,
    const std::string& currentPresetName, const std::string& presetFolder,
    std::function<void(int)> callback) {
    endCallback = callback;

    NSMenu* menu = [[NSMenu alloc] initWithTitle: @"Preset Menu"];
    MenuDelegate* delegate = [[MenuDelegate alloc] init];

    for (const auto& pair : items) {
        NSString* title = [NSString stringWithUTF8String:pair.second.c_str()];

        NSMenuItem* menuItem = [[NSMenuItem alloc] initWithTitle: title
                action: @selector(menuItemSelected:) keyEquivalent: @""];

        menuItem.target = delegate;
        menuItem.tag = pair.first;
        [menu addItem: menuItem];
    }

    if (!factoryItems.empty()) {
        NSMenu* factoryMenu = [[NSMenu alloc] initWithTitle:@"Factory"];

        for (const auto& pair : factoryItems) {
            NSString* title = [NSString stringWithUTF8String:pair.second.c_str()];
            NSMenuItem* factoryMenuItem = [[NSMenuItem alloc] initWithTitle:title
                action: @selector(menuItemSelected:) keyEquivalent: @""];

            factoryMenuItem.target = delegate;
            factoryMenuItem.tag = pair.first;
            if (presetFolder == "factory" && pair.second == currentPresetName) {
                [factoryMenuItem setState:NSControlStateValueOn];
            }

            [factoryMenu addItem:factoryMenuItem];
        }

        NSMenuItem* factoryMenuItem = [[NSMenuItem alloc] initWithTitle: @"Factory"
               action:nil keyEquivalent: @""];

        [factoryMenuItem setSubmenu: factoryMenu];
        [menu addItem: factoryMenuItem];
    }

    if (!userItems.empty()) {
        NSString* nsUserFolder = [NSString stringWithUTF8String: userFolder.c_str()];
        NSMenu* userMenu = [[NSMenu alloc] initWithTitle: nsUserFolder];

        for (const auto& pair : userItems) {
            NSString* title = [NSString stringWithUTF8String:pair.second.c_str()];
            NSMenuItem* userMenuItem = [[NSMenuItem alloc] initWithTitle:title
                action: @selector(menuItemSelected:) keyEquivalent: @""];

            userMenuItem.target = delegate;
            userMenuItem.tag = pair.first;
            if (presetFolder == "user" && pair.second == currentPresetName) {
                [userMenuItem setState:NSControlStateValueOn];
            }

            [userMenu addItem: userMenuItem];
        }

        NSMenuItem* userMenuItem = [[NSMenuItem alloc] initWithTitle: nsUserFolder
               action:nil keyEquivalent: @""];

        [userMenuItem setSubmenu: userMenu];
        [menu addItem: userMenuItem];
    }

    NSPoint mouseLocation = [NSEvent mouseLocation];
    const CGFloat estimatedMenuHeight = (items.size() + 2) * 22.0;
    const CGFloat estimatedMenuWidth = 110.0;

    mouseLocation.y += estimatedMenuHeight;
    mouseLocation.x -= estimatedMenuWidth / 2.0;

    [menu popUpMenuPositioningItem: nil atLocation:mouseLocation inView: nil];
}
