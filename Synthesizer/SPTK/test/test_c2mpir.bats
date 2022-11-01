#!/usr/bin/env bats
# ------------------------------------------------------------------------ #
# Copyright 2021 SPTK Working Group                                        #
#                                                                          #
# Licensed under the Apache License, Version 2.0 (the "License");          #
# you may not use this file except in compliance with the License.         #
# You may obtain a copy of the License at                                  #
#                                                                          #
#     http://www.apache.org/licenses/LICENSE-2.0                           #
#                                                                          #
# Unless required by applicable law or agreed to in writing, software      #
# distributed under the License is distributed on an "AS IS" BASIS,        #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. #
# See the License for the specific language governing permissions and      #
# limitations under the License.                                           #
# ------------------------------------------------------------------------ #

sptk3=tools/sptk/bin
sptk4=bin
tmp=test_c2mpir

setup() {
    mkdir -p $tmp
}

teardown() {
    rm -rf $tmp
}

@test "c2mpir: compatibility" {
    $sptk3/nrand -l 20 | $sptk3/c2ir -m 9 -l 32 > $tmp/1
    $sptk3/nrand -l 20 | $sptk4/c2mpir -m 9 -l 32 > $tmp/2
    run $sptk4/aeq $tmp/1 $tmp/2
    [ "$status" -eq 0 ]

    $sptk3/nrand -l 20 | $sptk4/mgc2mgc -g 0 -G 1 -m 9 -M 31 -U > $tmp/3
    run $sptk4/aeq $tmp/2 $tmp/3
    [ "$status" -eq 0 ]
}

@test "c2mpir: reversibility" {
    $sptk3/nrand -l 20 > $tmp/1
    $sptk4/c2mpir -m 9 -l 32 $tmp/1 | $sptk4/mpir2c -l 32 -M 9 > $tmp/2
    run $sptk4/aeq $tmp/1 $tmp/2
    [ "$status" -eq 0 ]
}

@test "c2mpir: valgrind" {
    $sptk3/nrand -l 20 > $tmp/1
    run valgrind $sptk4/c2mpir -m 9 -l 32 $tmp/1
    [ "$(echo "${lines[-1]}" | sed -r 's/.*SUMMARY: ([0-9]*) .*/\1/')" -eq 0 ]
}
