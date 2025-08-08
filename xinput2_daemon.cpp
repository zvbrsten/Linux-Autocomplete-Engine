#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/record.h>
#include <X11/keysym.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cctype>
#include <map>
#include "MMapTrieLoader.h"

MMapTrieLoader loader;
std::string currentPrefix;
Display* ctrl_dpy;

bool loadTrieData() {
    return loader.loadWordsOnly("data/words_blob.dat");
}

void typeString(Display *dpy, const std::string &text) {
    for (char c : text) {
        KeySym ks;
        if (c >= 'a' && c <= 'z') {
            ks = XK_a + (c - 'a');
        } else if (c >= 'A' && c <= 'Z') {
            ks = XK_A + (c - 'A');
        } else {
            continue;
        }
        
        KeyCode kc = XKeysymToKeycode(dpy, ks);
        if (kc != 0) {
            XTestFakeKeyEvent(dpy, kc, True, 0);
            XTestFakeKeyEvent(dpy, kc, False, 0);
            XFlush(dpy);
            usleep(5000);
        }
    }
}

void processKeyEvent(KeyCode keycode, bool pressed) {
    if (!pressed) return;
    
    static KeyCode tab_keycode = XKeysymToKeycode(ctrl_dpy, XK_Tab);
    static KeyCode space_keycode = XKeysymToKeycode(ctrl_dpy, XK_space);
    static KeyCode return_keycode = XKeysymToKeycode(ctrl_dpy, XK_Return);
    static KeyCode backspace_keycode = XKeysymToKeycode(ctrl_dpy, XK_BackSpace);
    
    static std::map<KeyCode, char> keymap;
    if (keymap.empty()) {
        for (char c = 'a'; c <= 'z'; c++) {
            KeyCode kc = XKeysymToKeycode(ctrl_dpy, XK_a + (c - 'a'));
            if (kc != 0) keymap[kc] = c;
        }
        for (char c = 'A'; c <= 'Z'; c++) {
            KeyCode kc = XKeysymToKeycode(ctrl_dpy, XK_A + (c - 'A'));
            if (kc != 0) keymap[kc] = tolower(c);
        }
    }
    
    if (keycode == tab_keycode) {
        std::cout << "Tab detected! Current prefix: '" << currentPrefix << "'" << std::endl;
        
        if (!currentPrefix.empty()) {
            auto suggestions = loader.queryPrefix(currentPrefix, 1);
            if (!suggestions.empty() && suggestions[0].first != currentPrefix) {
                std::string completion = suggestions[0].first.substr(currentPrefix.size());
                std::cout << "Completing with: '" << completion << "'" << std::endl;
                
                typeString(ctrl_dpy, completion);
                currentPrefix = "";
            }
        }
    } else if (keycode == space_keycode || keycode == return_keycode) {
        std::cout << "Word boundary detected, clearing prefix" << std::endl;
        currentPrefix.clear();
    } else if (keycode == backspace_keycode) {
        if (!currentPrefix.empty()) {
            currentPrefix.pop_back();
            std::cout << "Backspace, prefix now: '" << currentPrefix << "'" << std::endl;
        }
    } 
}

void recordCallback(XPointer closure, XRecordInterceptData* recorded_data) {
    if (recorded_data->category != XRecordFromServer) {
        XRecordFreeData(recorded_data);
        return;
    }
    
    
    unsigned char* data = (unsigned char*)recorded_data->data;
    
    
    int event_type = data[0] & 0x7F; 
    
    if (event_type == KeyPress) {
        
        KeyCode keycode = data[1];
        processKeyEvent(keycode, true);
    }
    
    XRecordFreeData(recorded_data);
}

int main() {
    ctrl_dpy = XOpenDisplay(NULL);
    
    if (!ctrl_dpy) {
        std::cerr << "Cannot open X display" << std::endl;
        return 1;
    }

    if (!loadTrieData()) {
        std::cerr << "Failed to load data from words_blob.dat" << std::endl;
        return 1;
    }

    
    int major, minor;
    if (!XRecordQueryVersion(ctrl_dpy, &major, &minor)) {
        std::cerr << "RECORD extension not supported" << std::endl;
        return 1;
    }

    Display* data_dpy = XOpenDisplay(NULL);
    if (!data_dpy) {
        std::cerr << "Cannot open data display" << std::endl;
        return 1;
    }

    
    XRecordClientSpec clients = XRecordAllClients;
    XRecordRange* range = XRecordAllocRange();
    if (!range) {
        std::cerr << "Could not allocate record range" << std::endl;
        return 1;
    }

    range->device_events.first = KeyPress;
    range->device_events.last = KeyRelease;

    XRecordContext context = XRecordCreateContext(data_dpy, 0, &clients, 1, &range, 1);
    if (!context) {
        std::cerr << "Could not create record context" << std::endl;
        return 1;
    }

    XFree(range);

    std::cout << "XRecord-based autocomplete daemon started." << std::endl;

    
    if (!XRecordEnableContext(data_dpy, context, recordCallback, NULL)) {
        std::cerr << "Could not enable record context" << std::endl;
        return 1;
    }

    
    XRecordDisableContext(ctrl_dpy, context);
    XRecordFreeContext(ctrl_dpy, context);
    XCloseDisplay(ctrl_dpy);
    XCloseDisplay(data_dpy);
    return 0;
}