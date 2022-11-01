// Copyright AudioKit. All Rights Reserved. Revision History at http://github.com/AudioKit/AudioKit/

#import <AudioToolbox/AudioToolbox.h>
#import "OscillatorBank_AU_iOSDSPKernelAdapter.h"

// Define parameter addresses.
extern const AudioUnitParameterID myParam1;

@interface OscillatorBank_AU_iOSAudioUnit : AUAudioUnit

@property (nonatomic, readonly) OscillatorBank_AU_iOSDSPKernelAdapter *kernelAdapter;
- (void)setupAudioBuses;
- (void)setupParameterTree;
- (void)setupParameterCallbacks;
@end
