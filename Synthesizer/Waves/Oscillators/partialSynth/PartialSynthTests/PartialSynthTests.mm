//
//  PartialSynthTests.m
//  PartialSynthTests
//
//  Created by Alex on 04/02/2018.
//  Copyright © 2018 Alex Nadzharov. All rights reserved.
//

#import <XCTest/XCTest.h>

#import "SinePartial.hpp"
#import "SineSynth.hpp"
#import "ResPartial.hpp"
#import "ResSynth.hpp"

@interface PartialSynthTests : XCTestCase

@end

@implementation PartialSynthTests

- (void)testBasic
{
    SinePartial sp;

    sp.set<SinePartial::pAmp>(0.5);
    sp.set<SinePartial::pFreq>(440);

    float* b = new float[512];

    sp.process(512, b);

    SineSynth s;

    SinePartialData d(1);
    d.amp[0] = 0.5;
    d.freq[0] = 440;

    s.setData(d);
    s.process(b, 512);

    delete[] b;

    ResPartial rp;

    rp.set<ResPartial::pGain>(0.5);
    rp.set<ResPartial::pFreq>(440);

    b = new float[512];
    float* bi = new float[512];
    sp.process(512, b);

    ResSynth r;

    ResPartialData rd(1);
    rd.gain[0] = 0.5;
    rd.freq[0] = 440;

    r.setData(rd);
    r.process(bi, b, 512);

    delete[] b;
}

@end


