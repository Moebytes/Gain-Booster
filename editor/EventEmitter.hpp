#pragma once
#include <JuceHeader.h>

class EventEmitter : private AsyncUpdater {
public:
    struct Listener {
        virtual ~Listener() = default;
        virtual auto handleEvent(const String& name, const var& payload) -> void = 0;
    };

    static auto instance() -> EventEmitter& {
        static EventEmitter emitter;
        return emitter;
    }

    auto addListener(Listener* listener) -> void { this->listeners.add(listener); }
    auto removeListener(Listener* listener) -> void { this->listeners.remove(listener); }

    auto emitEvent(const String& name, const var& payload) -> void {
        const ScopedLock scopedLock{this->lock};
        this->pendingEvents.add({name, payload});
        this->triggerAsyncUpdate();
    }

private:
    struct EventData {
        String name;
        var payload;
    };

    ListenerList<Listener> listeners;
    CriticalSection lock;
    Array<EventData> pendingEvents;

    EventEmitter() = default;

    auto handleAsyncUpdate() -> void override {
        const ScopedLock scopedLock{this->lock};
        Array<EventData> eventArray;
        eventArray.swapWith(this->pendingEvents);

        for (auto& event : eventArray) {
            this->listeners.call([&](Listener& listener) { listener.handleEvent(event.name, event.payload); });
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EventEmitter)
};