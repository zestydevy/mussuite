# MusSuite Internal Representations

As opposed to converting directly from the input format to libmus data, MusSuite first converts the input data into an internal representation of that data. This internal representation is then converted to libmus data on export and is designed to be format-agnostic to allow for supporting multiple input formats. This internal representation is not necessarily a thin wrapper around the libmus data, but it does have a similar scope as far as what parameters it contains (as opposed to the input formats which may be far more complex).

MusSuite's internal representation consists of a main data structure that contains a collection of audio tracks and a collection of instruments.

## Audio Tracks

Each audio track is a linear collection of events, where each event is a single piece of data that modifies the state of the channel when it is reached. Events include starting or stopping notes, changing the instrument that the channel is using, setting the tempo of the channel, and so on. Events may also have a time duration, which means the next event will be triggered after the duration of the previous event. This is used to give notes and rests their lengths.

## Events

Events in an audio channel have a type and some other set of parameters. The number of parameters, and therefore size, of an event can vary based on event type, but they all maintain an identical interface that they inherit from an abstract base class. This interface includes, among other to-be-determined methods, a method to output that event as the appropriate command to a libmus data stream. This method takes a reference to the main data structure and current channel state at the time of the event, so that the event can be aware of any additional commands it may need to add to the libmus data stream in addition to the direct one it maps to. This also allows note events to determine which sample to use in the output data.

Due to the polymorphic nature of Events, various types of Event cannot be stored directly in a collection. This leaves a few options, such as a collection of pointers to Event objects or some form of polymorphic collection (such as using std::variant as the element type). The former incurs many separate memory allocations and an extra layer of indirection, while the latter incurs a runtime penalty when determining what type each element is, as well as extra memory usage since every element will have to be as large as the largest type of Event. It is to be determined which method of storage will be used.

## Instruments

Instruments are a mapping of parameters (such as volume, pitch, velocity) to the corresponding sample. This is one area where MusSuite's internal representation heavily deviates from libmus' data format, as libmus songs only consider waveforms (samples) rather than a higher-level description of the instrument being played. During export, song data is processed by MusSuite such that instruments are collapsed to the specific sample used for each note in a given song based on the state of the audio channel they are in when played. This higher-level representation of instruments is more akin to what is found in formats such as sf2/sfz/xm making it easier to write importers, while still being straightforward to convert to libmus data on export.