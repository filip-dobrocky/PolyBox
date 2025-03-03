# PolyBox

Polyrhythmic step sequencer + sampler with microtuning ability created in JUCE.

Features:
- 6-voice polyrhythmic step sequencer with variable lengths
- ability to add 6 user samples
- variable sequencer-sampler connections
- microtuning achieved by loading .scl and .kbm files (using Surge Tuning Library - https://github.com/surge-synthesizer/tuning-library)
- real-time pattern transposition using MIDI input
- multichannel MIDI output

## TODO:
### HIGH priority
- state saving - config page
    - tuning
    - sequencer routing
- proper DAW sync - playhead
- MIDI input function improvement - control `playing` parameter by MIDI input
### MID priority
- UI improvements
    - change record button to midi input button
    - colors
    - logo
    - custom look and feel
    - prettier sequencer UI
- manual / tooltips
- multichannel output
### EXTRA
- random sample position
- envelope curves
- filters for each sample
- sequencing audio parameters