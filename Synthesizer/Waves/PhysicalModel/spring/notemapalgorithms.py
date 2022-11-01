"""
/*
 This file is part of Virtual Robot's Pine Synthesizer. 

Copyright <2022> <Brian Gunnison>

Permission is hereby granted, free of charge, to any person obtaining a copy of this 
software and associated documentation files (the "Software"), to deal in the Software 
without restriction, including without limitation the rights to use, copy, modify, 
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so, subject to the following 
conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

Note maps algorithms
"""

midi_notes = ['C',	'C#',	'D',	'D#',	'E',	'F',	'F#',	'G',	'G#',	'A',	'A#',	'B']
def run_maps():
    print('Major pentatonic')
    map = [0,2,5,6,9]
    l = len(map)
    root = 2
    for mn in range(127):
        base = int(mn/len(map))
        i = mn - (base * l) 
        o = map[i] + root + (base * 12)
        print(f'mn: {mn}, b: {base}, i: {i}, on: {o}, note: {n}')





if __name__ == "__main__":
    run_maps()


