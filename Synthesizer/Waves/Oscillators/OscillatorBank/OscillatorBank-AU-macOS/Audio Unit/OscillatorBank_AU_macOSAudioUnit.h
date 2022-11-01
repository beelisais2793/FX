// Copyright AudioKit. All Rights Reserved. Revision History at http://github.com/AudioKit/AudioKit/

#import <AudioToolbox/AudioToolbox.h>
#import "OscillatorBank_AU_macOSDSPKernelAdapter.h"

// Define parameter addresses.
extern const AudioUnitParameterID myParam1;

@interface OscillatorBank_AU_macOSAudioUnit : AUAudioUnit

@property (nonatomic, readonly) OscillatorBank_AU_macOSDSPKernelAdapter *kernelAdapter;
- (void)setupAudioBuses;
- (void)setupParameterTree;
- (void)setupParameterCallbacks;
@end
