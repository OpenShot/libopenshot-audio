// SPDX-FileCopyrightText: 2026 OpenShot Studios, LLC
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <cmath>
#include <iostream>
#include <memory>
#include "OpenShotAudio.h"

#if defined(REQUIRE_ARM64) && !defined(__aarch64__) && !defined(_M_ARM64)
#error The ARM64 smoke test must be compiled for ARM64
#endif

int main()
{
    // Exercise the installed library without requiring an audio device.
    juce::AudioBuffer<float> samples(2, 128);
    for (int i = 0; i < samples.getNumSamples(); ++i) {
        samples.setSample(0, i, 0.5f);
        samples.setSample(1, i, -0.25f);
    }
    samples.applyGain(0.5f);
    juce::MemoryBlock encoded;
    juce::WavAudioFormat format;
    auto output = std::make_unique<juce::MemoryOutputStream>(encoded, false);
    std::unique_ptr<juce::AudioFormatWriter> writer(
        format.createWriterFor(output.get(), 48000, 2, 16, {}, 0));
    if (!writer) return 1;
    output.release(); // The writer owns the stream after successful creation.
    if (!writer->writeFromAudioSampleBuffer(samples, 0, samples.getNumSamples())) return 1;
    writer.reset(); // Finalize the WAV header before reading it.
    std::unique_ptr<juce::AudioFormatReader> reader(format.createReaderFor(
        new juce::MemoryInputStream(encoded, false), true));
    if (!reader || reader->numChannels != 2 || reader->sampleRate != 48000
        || reader->lengthInSamples != 128) return 1;
    juce::AudioBuffer<float> decoded(2, 128);
    if (!reader->read(&decoded, 0, 128, 0, true, true)) return 1;
    for (int i = 0; i < decoded.getNumSamples(); ++i) {
        if (std::abs(decoded.getSample(0, i) - 0.25f) > 0.0001f
            || std::abs(decoded.getSample(1, i) + 0.125f) > 0.0001f) return 1;
    }
    std::cout << "Installed audio library: buffer gain and WAV round trip passed\n";
    return 0;
}
