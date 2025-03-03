# PolyBox

Polyrhythmic step sequencer + sampler with microtuning ability created in JUCE.

Features:
- 6-voice polyrhythmic step sequencer with variable lengths
- ability to add 6 user samples
- variable sequencer-sampler connections
- microtuning achieved by loading .scl and .kbm files (using Surge Tuning Library - https://github.com/surge-synthesizer/tuning-library)
- real-time pattern transposition using MIDI input
- multichannel MIDI output

TODO:
- ability to save plug-in state - parameters, presets
    - `SamplerSound` parameters - `gain`, `pan`, `attack`, `release`, `start`, `end`, `root` (`float`), `reversed` (`bool`)
    - `SequencerVoice` - `length`, `midiChannels`
    - tuning file paths
    - sample `sourcePath` (`string`)
    - Serialize `Sequencer`
- proper DAW sync
- UI improvements
- manual / tooltips
- multichannel output