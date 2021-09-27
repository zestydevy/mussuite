# Libmus Song Format

## File Header

Libmus songs have a flexible structure; the only fixed location for specific data is the header at the start of the file.
All data is big endian. A song file always starts with a header, which always contains the following 3 values:
* Version - 4 bytes 
* Number of channels - 4 bytes
* Number of waves (samples) - 4 bytes

The header also contains the following 7 values, which are offsets into the file.
They are converted into pointers by libmus when the song is loaded at runtime.
* Data List - 4 bytes
  * Points to the a list of offsets in the file, where each offset points to the song data for a single track
  * Specified offset must be 4-byte aligned
  * The list does not include the master track, and is [Number of channels] long
  * Each entry in the list is 4 bytes wide
* Volume List - 4 bytes
  * Points to the a list of offsets in the file, where each offset points to the volume data for a single track
  * Specified offset must be 4-byte aligned
  * The list does not include the master track, and is [Number of channels] long
  * Each entry in the list is 4 bytes wide
* Pitchbend List - 4 bytes
  * Points to the a list of offsets in the file, where each offset points to the pitchbend data for a single track
  * Specified offset must be 4-byte aligned
  * The list does not include the master track, and is [Number of channels] long
  * Each entry in the list is 4 bytes wide
* Envelope Table - 4 bytes
  * Points to the a list of envelopes in the file
  * Each entry in the table is 7 bytes long
* Drum Table - 4 bytes
  * Points to the a list of drums in the file
  * Each entry in the table is 6 bytes long and must be 2-byte aligned
* Wave Table - 4 bytes
  * Points to the a list of sample indices in the file, where each sample index references a sample in the loaded wave bank
  * Specified offset must be 2-byte aligned
  * The list is [Number of waves] long
  * Each entry in the list is 2 bytes wide
* Master Track - 4 bytes
  * Points to the song data for the master track

## Structures

### Drums
Drums are 6 bytes long:
* Bytes 0-1: Wave Number (Sample Index)
* Bytes 2-3: Envelope Number
* Byte 4:    Pan Position
* Byte 5:    Pitch (Semitone)

### Envelopes
Envelopes are 7 bytes long:
* Byte 0: Envelope Speed
* Byte 1: Initial Volume
* Byte 2: Attack Speed
* Byte 3: Peak Volume
* Byte 4: Decay Speed
* Byte 5: Sustain Volume
* Byte 6: Release Speed

### Pitchbend

## Sequences

### Songs
Songs are composed of a list of commands of varying lengths as well as notes. Notes and some command types have multiple encoding schemes based on what info they contain, and therefore may not always be the same length in the file. Commands affect playback of notes or act as flow control for the song's sequence.

Commands always have a first byte with a value greater than 0x7F, and everything else is treated as a note.

A note can be between 1 and 4 bytes long.

It starts with a byte containing the pitch of the note to play, or 0x60 if it's a rest.

If velocity is on (via `Fvelon`), then the next byte will be the velocity of the note. If velocity is off (via `FVeloff` or `Fvelocity`), then the note's velocity will be set to the channel's default velocity, which is either the velocity of the last note while velocity was on, the value set by `Fvelocity`, or 0x7F if velocity was not set yet on this channel.

If the channel does not have a fixed length (via `Flength`) and the channel is not currently ignoring fixed lengths (via `Fignore`), the next byte of the note is its length. If the highest bit is unset (i.e. the length is less than 0x80), then the length is used as is. If the highest bit is set, then the high bit is maked out of this byte and used as the MSB of velocity, with the next byte being used as the LSB. e.g. hex `73` would be a length of 0x73, and hex `A2 41` would be a length of 0x2241. This encoding scheme is used in some commands as well.

There are 45 commands in the libmus song format:
* 0x80 - `Fstop`
  * Stops a channel
  * Length - 1 byte
* 0x81 - `Fwave`
  * Sets the wave number (sample index) to use for notes in this channel
  * Length - 2 or 3 bytes
  * Parameters
    * Wave number - 1 or 2 bytes (encoded like note length)
* 0x82 - `Fport`
  * Enables portamento
  * Length - 2 bytes
  * Parameters
    * Portamento - 1 byte
* 0x83 - `Fportoff`
  * Disables portamento
  * Length - 1 byte
* 0x84 - `Fdefa`
  * Sets the channel's envelope
  * Length - 8 bytes
  * Parameters (same as envelopes in the envelope table)
    * Envelope Speed - 1 byte
    * Initial Volume - 1 byte
    * Attack Speed - 1 byte
    * Peak Volume - 1 byte
    * Decay Speed - 1 byte
    * Sustain Volume - 1 byte
    * Release Speed - 1 byte
* 0x85 - `Ftempo`
  * Sets the channel's tempo
  * Length - 2 bytes
  * Parameters
    * Tempo (beats per minute)
* 0x86 - `Fcutoff`
* 0x87 - `Fendit`
* 0x88 - `Fvibup`
* 0x89 - `Fvibdown`
* 0x8A - `Fviboff`
* 0x8B - `Flength`
  * Sets the channel's fixed length, overriding note lengths (can be disabled for a single note with `Fignore`)
  * Length - 2 or 3 bytes
  * Parameters
    * Length - 1 or 2 bytes (encoded like note length)
* 0x8C - `Fignore`
  * Ignores a channel's fixed length for one note
  * Length - 1 byte
* 0x8D - `Ftrans`
  * Sets a channel's transpose value (pitch offset)
  * Length - 2 bytes
  * Parameters
    * Transpose - 1 byte (s8)
* 0x8E - `Fignore_trans`
  * Permanently disables a channel's transpose
* 0x8F - `Fdistort`
* 0x90 - `Fenvelope`
  * Sets a channel's envelope from the envelope table
  * Length - 2 or 3 bytes
  * Parameters
    * Envelope Index - 1 or 2 bytes (encoded like note length)
* 0x91 - `Fenvoff`
  * Disables envelopes for this channel
  * Length - 1 byte
* 0x92 - `Fenvon`
  * Enables envelopes for this channel
  * Length - 1 byte
* 0x93 - `Ftroff`
* 0x94 - `Ftron`
* 0x95 - `Ffor`
  * Marks the start point of a loop and sets how many times to run the loop
    * A loop count of 0xFF means loop infinitely
  * Length - 2 bytes
  * Parameters
    * Loop Count - 1 byte
* 0x96 - `Fnext`
  * Marks the end point of a loop
  * Length - 1 byte
* 0x97 - `Fwobble`
* 0x98 - `Fwobbleoff`
* 0x99 - `Fvelon`
* 0x9A - `Fveloff`
* 0x9B - `Fvelocity`
* 0x9C - `Fpan`
  * Sets the channel's pan
    * Pan is between 0x00, which is all the way left (?) and 0xFF which is all the way right (?)
  * Length - 2 bytes
  * Parameters
    * Pan Position
* 0x9D - `Fstereo`
  * IGNORED
  * Length - 3 bytes
* 0x9E - `Fdrums`
  * Enables drums for a channel, which replaces note playback with playing the corresponding drum instead
    * e.g. A note with pitch 0 would play drum 0 instead
    * Accepts an offset value which offsets drum index from pitch, so an offset of 1 and pitch of 0 would play drum 1 instead
  * Length - 2 or 3 bytes
  * Parameters
    * Drum Offset - 1 or 2 bytes (encoded like note length)
* 0x9F - `Fdrumsoff`
  * Disables drums for a channel
  * Length - 1 byte
* 0xA0 - `Fprint`
 * IGNORED (debug only)
 * Length - 2 bytes
* 0xA1 - `Fgoto`
* 0xA2 - `Freverb`
* 0xA3 - `FrandNote`
  * Sets the channel's transpose to a random value in a given range [0, R) with an offset O
    * e.g. Range = 20 and Offset = -3 means a value between -3 and 16 inclusive
  * Length - 3 bytes
  * Parameters
    * Range - 1 byte
    * Offset - 1 byte (effectively s8)
* 0xA4 - `FrandVolume`
  * Sets the channel's volume to a random value in a given range [0, R) with an offset O
    * Same calculation as `FrandNote`
  * Length - 3 bytes
  * Parameters
    * Range - 1 byte
    * Offset - 1 byte (effectively s8)
* 0xA5 - `FrandPan`
  * Sets the channel's pan to a random value in a given range [0, R) with an offset O
    * Same calculation as `FrandNote`
  * Length - 3 bytes
  * Parameters
    * Range - 1 byte
    * Offset - 1 byte (effectively s8)
* 0xA6 - `Fvolume`
  * Sets the channel's volume
  * Length - 2 bytes
  * Parameters
    * Volume - 1 byte
* 0xA7 - `Fstartfx`
  * Plays a sound effect from the loaded fx bank with the channel's current parameters (volume, transpose, etc.)
  * Length - 2 or 3 bytes
  * Parameters
    * Sound effect index - 1 or 2 bytes (encoded like note length)
* 0xA8 - `Fbendrange`
* 0xA9 - `Fsweep`
* 0xAA - `Fchangefx`
* 0xAB - `Fmarker`
  * Calls a user-provided callback when this point in the song is reached (only works on master track)
  * Length - 3 or 4 bytes
  * Parameters
    * Marker number - 1 byte
    * Rest (unused) - 1 or 2 bytes (encoded like note length)
* 0xAC - `Flength0`
  * Disables a channel's fixed length override, equivalent to `Flength 0`
  * Length - 1 byte

### Volumes
TODO

### Pitchbends
TODO