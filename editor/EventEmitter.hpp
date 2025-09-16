#pragma once
#include <JuceHeader.h>

class EventEmitter : private juce::AsyncUpdater {
public:
    struct Listener {
        virtual ~Listener() = default;
        virtual auto handleEvent(const juce::String& name, const juce::var& payload) -> void = 0;
    };

    static auto instance() -> EventEmitter& {
        static EventEmitter emitter;
        return emitter;
    }

    auto addListener(Listener* listener) -> void { this->listeners.add(listener); }
    auto removeListener(Listener* listener) -> void { this->listeners.remove(listener); }

    auto emitEvent(const juce::String& name, const juce::var& payload) -> void {
        const juce::ScopedLock scopedLock{this->lock};
        this->pendingEvents.add({name, payload});
        this->triggerAsyncUpdate();
    }

private:
    struct EventData {
        juce::String name;
        juce::var payload;
    };

    juce::ListenerList<Listener> listeners;
    juce::CriticalSection lock;
    juce::Array<EventData> pendingEvents;

    EventEmitter() = default;

    auto handleAsyncUpdate() -> void override {
        const juce::ScopedLock scopedLock{this->lock};
        juce::Array<EventData> eventArray;
        eventArray.swapWith(this->pendingEvents);

        for (auto& event : eventArray) {
            this->listeners.call([&](Listener& listener) { listener.handleEvent(event.name, event.payload); });
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EventEmitter)
};